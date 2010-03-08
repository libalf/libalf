/* $Id:
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

/*
 * Kearns and Vazirani is an online learning algorithm described in
 *	* M.J. Kearns, U.V. Vazirani - An Introduction to Computational Learning Theory, MIT Press, 1994.
 */

#include <list>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <stdlib.h>
 
#include <libalf/knowledgebase.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

template <class answer>
class kearns_vazirani : public learning_algorithm<answer> {
	friend class task;

	private:
	
	//========== Definition of nodes ===========================================
	
	/*
	 * Basic class of a node.
	 */
	class node {
		
		public:
		
		list<int> label;	// The label of the node
		node *parent;		// Pointer to the node's parent (or NULL if root node)
		int level;			// The depth of the node in the tree (root has level 0)
		
		/* 
		 * Creates a new node.
		 */ 
		node (list<int> &label, int level) {
			this->label = label;
			this->level = level;
		}
		
		/*
		 * Dummy destructor
		 */
		virtual ~node() {}
		
		/*
		 * Checks whether the node is a leaf node.
		 */
		virtual bool is_leaf() = 0;
	};

	/*
	 * An inner node
	 */ 
	class inner_node : public node {
		public:
		node *left_child, *right_child;	// Left and rigt child of this inner node.
		
		/*
		 * Creates a new inner node.
		 *
		 * Takes A label, a level and its left and right children as arguments.
		 */
		inner_node (list<int> &label, int level, node *left_child, node *right_child)
		: node (label, level) {
			this->left_child = left_child;
			this->right_child = right_child;
		}
		
		/*
		 * Destructor
		 */
		~inner_node () {
			delete left_child;
			delete right_child;
		}
		
		bool is_leaf() {
			return false;
		}
	};
	
	/*
	 * A leaf node
	 */
	class leaf_node : public node {
		public:
		bool accepting;							// Stores whether the node is in the left or right subtree of the root
		set<leaf_node*> incoming_transitions;	// Stores all transitions pointing to this node
		leaf_node** transitions;				// The transitions of this node
		int id;									// A unique id to identify this equivalence class
		
		/*
		 * Creates a new leaf node.
		 *
		 * Takes a label, a level, an id, the accepting condition of the currently
		 * used alphabet size as parameters.
		 */
		leaf_node (list<int> &label, int level, int id, bool accepting, int alphabet_size)
		: node (label, level) {
			this->id = id;
			this->accepting = accepting;
			this->transitions = (leaf_node**)calloc(alphabet_size, sizeof(leaf_node*));
		}
		
		/*
		 * Descrutor
		 */
		~leaf_node() {
			free(transitions);
		}
		
		bool is_leaf() {
			return true;
		}
	};
	
	//========== Definitions of tasks ==========================================

	/*
	 * Root class of all tasks
	 */
	class task {
		public:
		task *prev, *next;
		
		/*
		 * Performs the task.
		 *
		 * Returns true if finished or false if information (i.e. membership queries
		 * are missing). An additional call to this method resume the operation.
		 *
		 * This method needs to be implemented be the specific tasks.
		 */
		virtual bool perform() = 0;
		
		/*
		 * Dummy destructor
		 */
		virtual ~task() {}
		
		/*
		 * Returns a string representation of this task.
		 */
		virtual string to_string() = 0;
	};
	
	/*
	 * Compute transition task.
	 *
	 * This task computes a transition of a leaf node (i.e. an equivalence class
	 * of the conjecture). This is done by "sifting the transition down the tree".
	 */
	class compute_transition_task : public task {
		private:
		leaf_node *source;				// The source of the transition
		node *current_node;				// The node currently reached during the sift operation
		int symbol;						// The transition symbol (i.e. the character)
		list<int> *transition_label;	// The label to sift down the tree
		kearns_vazirani *kv;			// Pointer to the learning algorithms (used to access the tree)

		public:
		
		/*
		 * Creates a new compute_transition_task.
		 *
		 * The constructor takes the source node, the symbol (i.e. the character)
		 * of the transition to compute, and a pointer to the learning algorithm
		 * as parameters.
		 */
		compute_transition_task(leaf_node *source, int symbol, kearns_vazirani *kv) {
			// Store parameter
			this->source = source;
			this->symbol = symbol;
			this->kv = kv;
			current_node = kv->root;
			
			// Create transition label
			transition_label = new list<int>(source->label);
			transition_label->push_back(symbol);
		}
		
		/*
		 * Destructor
		 */
		~compute_transition_task() {
			delete transition_label;
		}
		
		/*
		 * Performs the task.
		 */
		bool perform() {
		
			// Sift the transition down the tree
			do {

				// Query the 
				list<int> query(*transition_label);
				query.insert(query.end(), current_node->label.begin(), current_node->label.end());
				answer a;
				if(!kv->my_knowledge->resolve_or_add_query(query, a))
					return false;
				
				inner_node *inner = dynamic_cast<inner_node*> (current_node);
				
				if(a == true)
					current_node = inner->right_child;
				else
					current_node = inner->left_child;
				
			} while (!current_node->is_leaf());
			
			// Set transition
			leaf_node *leaf = dynamic_cast<leaf_node*> (current_node);
			source->transitions[symbol] = leaf;
			leaf->incoming_transitions.insert(source);
			
			// Memory cleanup
			delete transition_label;
			transition_label = NULL;
			
			return true;
		}
		
		/*
		 * Returns a string representation of this task.
		 */
		string to_string() {
			stringstream descr;
			descr << "\"";
			list<int>::iterator it;
			for(it = source->label.begin(); it != source->label.end(); it++)
				descr << (*it) << " ";
			descr << "\"-\"" << symbol << "\"-> ? (\"";
			for(it = current_node->label.begin(); it != current_node->label.end(); it++)
				descr << (*it) << " ";
			descr << "\")" << endl;
			return descr.str();
		}
	};
	
	/*
	 * Add counter-example task using a linar search for a "bad prefix".
	 *
	 * This task adds a counter-example. The counter-example is scanned from the
	 * beginning and when a "bad prefix" is descovered, the respective node is split.
	 * Thereto, this task creates a split_node_task.
	 */
	class add_counterexample_linearsearch_task : public task {
		protected:
		list<int> counterexample;	// The counter-example
		unsigned int position;		// The length of the prefix
		list<int> *prefix;			// The currently analyzed prefix of the counter-example
		node *sift_node;			// The current node reached on sifting the prefix
		kearns_vazirani *kv;		// Pointer to the learning algorithms (used to access the tree)
		

		public:
		
		/*
		 * Creates a new add_counterexample_simple_task.
		 *
		 * The constructor takes the the counter-example and a pointer to the
		 * learning algorithm as parameters.
		 */
		add_counterexample_linearsearch_task(const list<int> &counterexample, kearns_vazirani *kv) {
			// Store and initialize parameters
			this->counterexample = counterexample;
			this->sift_node = kv->root;
			this->kv = kv;
			position = 1;
			this->prefix = NULL;
			
			// Make initial prefix
			make_prefix();
		}

		/*
		 * Descrutor
		 */
		~add_counterexample_linearsearch_task() {
			delete prefix;
		}

		/*
		 * Performs the task.
		 */
		bool perform() {
			do {
				
				// If we have finished sifting, we need to check the next prefix
				if (sift_node->is_leaf()) {
					sift_node = kv->root;
					make_prefix();
				}

				// Get the leaf node that represents the equivalence class of the prefix
				do {
					
					// Perform membership query
					list<int> query (*prefix);
					query.insert(query.end(), sift_node->label.begin(), sift_node->label.end());
					answer a;
					if(!kv->my_knowledge->resolve_or_add_query(query, a))
						return false;
					
					// Sift it
					inner_node *inner = dynamic_cast<inner_node*>(sift_node);
					if (a == true)
						sift_node = inner->right_child;
					else
						sift_node = inner->left_child;

				} while(!sift_node->is_leaf());
				
				// Get the leaf node representing the state reached in the hypthesis
				// after reading the prefix
				leaf_node *run_node = kv->simulate_run(*prefix);
				
				// Check for bad prefix
				if (run_node != sift_node) {

					// Get next to last state of the run
					list<int> next_to_last;
					list<int>::iterator it;
					it = counterexample.begin();
					for(unsigned int i=1; i<=position-1; i++) {
						next_to_last.push_back(*it);
						it++;
					}
					leaf_node *node_to_split = kv->simulate_run(next_to_last);

					// Create parameter for split node task
					list<int> inner_node_label;
					node *lca = kv->least_commont_ancestor(run_node, sift_node);
					inner_node_label.push_back(*it);
					for(it = lca->label.begin(); it != lca->label.end(); it++)
						inner_node_label.push_back(*it);
					
					// Create and add split node task
					task *t = new split_node_task(node_to_split, next_to_last, inner_node_label, kv);
					
					kv->tasks.add_last(t);

					delete this->prefix;
					prefix = NULL;
					
					return true;
				}
				
				delete this->prefix;
				prefix = NULL;
			
			} while (next_position());

			// No bad prefix found. Log the error!
			(*kv->my_logger)(LOGGER_WARN, "kearns_vazirani: Found no bad prefix of the counter-example!\n");
			
			return true;
		}
		
		/*
		 * Returns a string representation of this task.
		 */
		string to_string() {
			stringstream descr;
			descr << "Add counter-example task linear search (counter example: \"";
			
			list<int>::iterator it;
			for(it = counterexample.begin(); it != counterexample.end(); it++)
				descr << (*it) << " ";
			descr << "\")";
			
			return descr.str();
		}
		
		private:
	
		/*
		 * Computes the next prefix of the counter-example to check.
		 */
		bool next_position() {
			if(this->position < counterexample.size()) {
				this->position++;
				return true;
			}
			else
				return false;
		}
		
		void make_prefix() {
			this->prefix = new list<int>;
			list<int>::iterator it;
			it = counterexample.begin();
			for(unsigned int i=1; i<=this->position; i++) {
				prefix->push_back(*it);
				it++;
			}
		}
	};
	
	
	/*
	 * Add counter-example task using a binary search for a "bad prefix".
	 *
	 * This task adds a counter-example. The counter-example is scanned using a
	 * binary search and when a "bad prefix" is descovered, the respective node
	 * is split. Thereto, this task creates a split_node_task.
	 */
	class add_counterexample_binarysearch_task : public task {
	
		private:
		list<int> counterexample;		// The counter-example
		unsigned int position;			// The length of the prefix
		list<int> *prefix;				// The currently analyzed prefix of the counter-example (up to position)
		list<int> *prefix_m1;			// The prefix up to position  - 1
		kearns_vazirani *kv;			// Pointer to the learning algorithms (used to access the tree)	
		unsigned int left, right;		// Left and right bounderies of the prefix
		node **sift_buffer;				// Buffer to store nodes during a sift operation (and finally the result)
		leaf_node **run_buffer;			// Buffer to store results of simulations
		
	
		public:
		
		/*
		 * Constructor: 
		 */
		add_counterexample_binarysearch_task(list<int> &counterexample, kearns_vazirani *kv) {
			// Store and initialize parameters
			this->counterexample = counterexample;
			this->kv = kv;
			sift_buffer = new node*[counterexample.size() + 1];
			run_buffer = new leaf_node*[counterexample.size() + 1];
			for(unsigned int i=1; i<=counterexample.size(); i++) {
				sift_buffer[i] = kv->root;
				run_buffer[i] = NULL;
			}
			sift_buffer[0] = run_buffer[0] = kv->initial_state;
			prefix = prefix_m1 = NULL;
			
			// Set bounderies
			left = 1;
			right = counterexample.size();
			
			// Compute initial position
			this->position = (left + right) / 2;
						
			// Create initial prefix
			this->make_prefix();
		}
	
		/*
		 * Destructor
		 */
		~add_counterexample_binarysearch_task() {
			delete prefix;
			delete prefix_m1;
			delete[] sift_buffer;
			delete[] run_buffer;
		}
	
		/*
		 * Performs the task.
		 */
		bool perform() {
			bool perform_loop = true;
			
			do {
				
				/*
				 * Get the leaf nodes that represents the equivalence classes of the prefixes.
				 */
				// Position i
				while(!sift_buffer[position]->is_leaf()) {
					list<int> query (*prefix);
					query.insert(query.end(), sift_buffer[position]->label.begin(), sift_buffer[position]->label.end());
					answer a;
					
					if(!kv->my_knowledge->resolve_or_add_query(query, a))
						break;
					
					// Sift it
					inner_node *inner = dynamic_cast<inner_node*>(sift_buffer[position]);
					if (a == true)
						sift_buffer[position] = inner->right_child;
					else
						sift_buffer[position] = inner->left_child;
				}
				
				// Position i - 1
				while(!sift_buffer[position - 1]->is_leaf()) {
					list<int> query (*prefix_m1);
					query.insert(query.end(), sift_buffer[position - 1]->label.begin(), sift_buffer[position - 1]->label.end());
					answer a;
					
					if(!kv->my_knowledge->resolve_or_add_query(query, a))
						break;
					
					// Sift it
					inner_node *inner = dynamic_cast<inner_node*>(sift_buffer[position - 1]);
					if (a == true)
						sift_buffer[position - 1] = inner->right_child;
					else
						sift_buffer[position - 1] = inner->left_child;
				}

				// Is all information available
				if(!(sift_buffer[position]->is_leaf() && sift_buffer[position - 1]->is_leaf()))
					return false;
				
				/*
				 * Get the leaf node representing the state reached in the hypthesis
				 * after reading the prefix.
				 */
				
				// Position i
				if(run_buffer[position] == NULL)
					run_buffer[position] = kv->simulate_run(*prefix);

				// Position i - 1
				if(run_buffer[position - 1] == NULL)
					run_buffer[position - 1] = kv->simulate_run(*prefix_m1);

				/*
				 * Check for bad prefix.
				 */
				if ((run_buffer[position - 1] == sift_buffer[position - 1]) && (run_buffer[position] != sift_buffer[position])) {

					// Create parameter for split node task
					list<int> inner_node_label;
					node *lca = kv->least_commont_ancestor(run_buffer[position], sift_buffer[position]);
					inner_node_label.push_back(*prefix->rbegin());
					inner_node_label.insert(inner_node_label.end(), lca->label.begin(), lca->label.end());
					
					// Create new leaf node label
					list<int> leaf_node_label(*prefix_m1);
					
					// Create and add split node task
					task *t = new split_node_task(run_buffer[position - 1], leaf_node_label, inner_node_label, kv);
					kv->tasks.add_last(t);

					// Memory cleanup
					delete this->prefix;
					delete this->prefix_m1;
					prefix = prefix_m1 = NULL;
					delete[] sift_buffer;
					delete[] run_buffer;
					sift_buffer = NULL;
					run_buffer = NULL;
					
					return true;
				}
			
				/*
				 * No bad prefix found!
				 */
				else {
					// Delete prefixes
					delete this->prefix;
					delete this->prefix_m1;
					prefix = prefix_m1 = NULL;
					
					if(left < right) {
						// Adjust position
						if(run_buffer[position] == sift_buffer[position])
							left = position + 1;
						else
							right = position - 1;
						position = (left + right) / 2;
						
						// Create new prefixes
						make_prefix();
					} else {
						perform_loop = false;
					}
				}
			
			} while (perform_loop);

			// Memory cleanup
			delete[] sift_buffer;
			delete[] run_buffer;
			sift_buffer = NULL;
			run_buffer = NULL;
			
			// No bad prefix found. Log the error!
			(*kv->my_logger)(LOGGER_WARN, "kearns_vazirani: Found no bad prefix of the counter-example!\n");
			
			return true;
		}
	
		/*
		 * Returns a string representation of this task.
		 */
		string to_string() {
			stringstream descr;
			descr << "Add counter-example task linear search (counter example: \"";
			
			list<int>::iterator it;
			for(it = counterexample.begin(); it != counterexample.end(); it++)
				descr << (*it) << " ";
			descr << "\")";
			
			return descr.str();
		}
	
		private:
		
		void make_prefix() {
			// Create new prefixes
			this->prefix = new list<int>;
			this->prefix_m1 = new list<int>;
			
			// Create prefix up to position - 1
			list<int>::iterator it;
			it = counterexample.begin();
			for(unsigned int i=1; i<=this->position - 1; i++) {
				prefix->push_back(*it);
				prefix_m1->push_back(*it);
				it++;
			}
			
			// Finish the prefix
			prefix->push_back(*it);
		}
	};
	
	/*
	 * Split node task
	 *
	 * This task splits a leaf node of the tree. Thereby, the node to split is
	 * "recycled".
	 *
	 * To preserve the soundness of the conjecture, the transitions pointing to
	 * the old node and the transitions of the new node have to be recalculated.
	 */
	class split_node_task : public task {
		private:
		leaf_node *node_to_split;			// The node to split
		list<int> new_leaf_node_label;		// The new leaf node label
		list<int> new_inner_node_label;		// The new inner node label
		kearns_vazirani *kv;				// Pointer to the learning algorithms (used to access the tree)

		public:
		
		/*
		 * Creates a new split_node_task.
		 *
		 * Takes a pointer to the node to split, the labels of the new leaf and
		 * inner nodes, and the pointer to the learning algorithm as arguments.
		 */
		split_node_task(leaf_node *node_to_split, list<int> &new_leaf_node_label, list<int> &new_inner_node_label, kearns_vazirani *kv) {
			this->node_to_split = node_to_split;
			this->new_leaf_node_label = new_leaf_node_label;
			this->new_inner_node_label = new_inner_node_label;
			this->kv = kv;
		}

		/*
		 * Descructor
		 */
		split_node_task() {
			// Nothing to do
		}
		
		/*
		 * Performs the task.
		 */
		bool perform() {
			
			// Query information about where to put the children
			answer a;
			list<int> query (new_leaf_node_label.begin(), new_leaf_node_label.end());
			query.insert(query.end(), new_inner_node_label.begin(), new_inner_node_label.end());
			if(!kv->my_knowledge->resolve_or_add_query(query, a))
				return false;

			// New child node
			leaf_node *new_child_node = new leaf_node(new_leaf_node_label, node_to_split->level + 1, kv->leaf_node_count, node_to_split->accepting, kv->alphabet_size);
			kv->leaf_node_count = kv->leaf_node_count + 1;
			
			// New inner node
			inner_node *new_inner_node;
			if (a == true)
				new_inner_node = new inner_node(new_inner_node_label, node_to_split->level, node_to_split, new_child_node);
			else
				new_inner_node = new inner_node(new_inner_node_label, node_to_split->level, new_child_node, node_to_split);

			// Update level of node to split
			node_to_split->level = node_to_split->level + 1;

			// Redirect node to split
			inner_node *parent = dynamic_cast<inner_node*>(node_to_split->parent);
			if (parent->left_child == node_to_split) {
				parent->left_child = new_inner_node;
			} else {
				parent->right_child = new_inner_node;
			}
			new_inner_node->parent = node_to_split->parent;
			node_to_split->parent = new_child_node->parent = new_inner_node;

			// Redirect transitions that point to the old leaf node
			typename set<leaf_node*>::iterator transition_it;
			for(transition_it = node_to_split->incoming_transitions.begin(); transition_it != node_to_split->incoming_transitions.end(); transition_it++) {
				for (int i=0; i<kv->alphabet_size; i++) {
					if ((*transition_it)->transitions[i] == node_to_split) {
						task *t = new compute_transition_task(*transition_it, i, kv);
						kv->tasks.add_last(t);
					}
				}
			}

			// Delete all incomming transitions
			node_to_split->incoming_transitions.clear();

			// Add transitions for new leaf node
			for (int i=0; i<kv->alphabet_size; i++) {
				task *t = new compute_transition_task(new_child_node, i, kv);
				kv->tasks.add_last(t);
			}

			return true;
		}
		
		/*
		 * Returns a string representation of this task.
		 */
		string to_string() {
			stringstream descr;
			descr << "split_node_task: node to split=\"";
			list<int>::iterator it;
			for(it = node_to_split->label.begin(); it != node_to_split->label.end(); it++)
				descr << (*it) << " ";
			descr << "\", leaf label=\"";
			for(it = new_leaf_node_label.begin(); it != new_leaf_node_label.end(); it++)
				descr << (*it) << " ";
			descr << "\", inner label=\"";
			for(it = new_inner_node_label.begin(); it != new_inner_node_label.end(); it++)
				descr << (*it) << " ";
			descr << "\")";
			
			return descr.str();
		}
	};
	
	//========== List of tasks =================================================
	
	/*
	 * This class realizes a list of tasks.
	 */
	class task_list {
	
		// First and last tasks
		private:
		task *first, *last;
		
		public:
		
		/*
		 * Create a new empty list.
		 */
		task_list () {
			first = last = NULL;
		}

		~task_list () {
			task *t = first;
			while(t != NULL) {
				task *tmp = t->next;
				delete t;
				t = tmp;
			}
		}
		
		/*
		 * Chechs whether the list is empty.
		 */
		bool is_empty() {
			return first == NULL;
		}
		
		/*
		 * Returns the first element of the list.
		 */
		task *get_first() {
			return first;
		}
		
		/*
		 * Returns the number of elements in the list.
		 */
		int size() {
			int i=0;
			task *t = first;
			while(t != NULL) {
				t = t->next;
				i++;
			}
			
			return i;
		}
		
		/*
		 * Adds a new task at the end of the list.
		 */
		void add_last(task *t) {
			if(last == NULL) {
				first = last = t;
				t->prev = t->next = NULL;
			} else {
				t->prev = last;
				t->next = NULL;
				last->next = t;
				last = t;			
			}
		}
		
		/*
		 * Removes a task from the list.
		 */
		void remove(task *t) {
			if (first == NULL)
				return;

			if (first == last) {
				if(first == t) {
					first = last = NULL;
					t->prev = t->next = NULL;
					return;
				}
			}

			if (t == first) {
				first = first->next;
				first->prev = NULL;
				t->prev = t->next = NULL;
			} else if (t == last) {
				last = last->prev;
				last->next = NULL;
				t->prev = t->next = NULL;
			} else {
				t->prev->next = t->next;
				t->next->prev = t->prev;
				t->prev = t->next = NULL;
			}
		}
		
		/*
		 * Returns a string representation of the list.
		 */
		string to_string() {
			stringstream descr;
			task *t = first;
			while(t != NULL) {
				descr << t->to_string();
				t = t->next;
			}
			
			return descr.str();
		}
		
	};
	
	//========== Variables =====================================================
	
	// Number of nodes in the tree
	int leaf_node_count, inner_node_count;
	
	// The root of the tree
	inner_node *root;
	
	// The initial state of the hypothesis
	leaf_node *initial_state;
	
	// Is the algorithm in the initial phase?
	bool initial_phase;
	
	// The task list
	task_list tasks;
	
	// Which technique to use when processing counter-examples?
	bool use_binary_search;
	
	public:
	
	//========== Constructors ==================================================
	
	/*
	 * Creates a new Kearns / Vazirani learning algorithm
	 */
	kearns_vazirani (knowledgebase<answer> *base, logger *log, int alphabet_size, bool use_binary_search = true) {
		// First store the parameters
		this->set_alphabet_size(alphabet_size);
		this->set_logger(log);
		this->set_knowledge_source(base);
		this->use_binary_search = use_binary_search;
		
		// Initial variables
		initial_phase = true;
		this->root = NULL;
	}
	
	/*
	 * Creates a new Kearns / Vazirani learning algorithm
	 */
	kearns_vazirani (knowledgebase<answer> *base, int alphabet_size) {
		// First store the parameters
		this->set_alphabet_size(alphabet_size);
		this->set_knowledge_source(base);
		this->use_binary_search = true;
		
		// Initial variables
		initial_phase = true;
		this->root = NULL;
	}
	
	/*
	 * Destructor
	 */ 
	~kearns_vazirani() {
		if(root)
			delete root;
	}
	
	//========== Methods =======================================================
	
	/*
	 * Returns whether binary (true) or a linar search (false) is performed to
	 * find bad prefixes of a counter-example.
	 */
	bool uses_binary_search() {
		return this->use_binary_search;
	}
	
	/*
	 * Sets whether to use a binary (true) or a linar search (false) to
	 * find bad prefixes of a counter-example.
	 */
	void set_binary_search(bool use_binary_search) {
		this->use_binary_search = use_binary_search;
	}
	
	/*
	 * Returns the number of leaf node of the tree.
	 */
	int get_leaf_node_count() {
		return leaf_node_count;
	}
	
	/*
	 * Returns the number of inner node of the tree.
	 */
	int get_inner_node_count() {
		return inner_node_count;
	}
	
	/*
	 * Advance one step in the algorithm.
	 */
	conjecture * advance() {
		conjecture * ret = NULL;
		
		this->start_timing();

		/*
		 * We are in the initial phase!
		 */
		if (initial_phase) {
			
			list<int> epsilon;
			answer a;
			
			if(this->my_knowledge->resolve_or_add_query(epsilon, a))
				ret = derive_conjecture();

		} 
		
		/*
		 * We are in the normal phase!
		 */
		else {
			
			// Let's go to work and process all pending tasks!
			task *current_task = tasks.get_first();
			
			while (current_task != NULL) {
					// Perform task
					bool task_finished = current_task->perform();
					
					task *old_task = current_task;

					// Next task
					current_task = current_task->next;

					// Delete task
					if (task_finished) {
						tasks.remove(old_task);
						delete old_task;
					}			
			}

			if (tasks.is_empty())
				ret = derive_conjecture();
		}

		this->stop_timing();

		return ret;
	}
		
	void increase_alphabet_size(int new_size) {
		(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: Increasing alphabet size is not supported by this implementation!\n");
	}
	
	memory_statistics get_memory_statistics() {
		// get_memory_statistics() is obsolete and will be removed in the future.
		// use receive_generic_statistics() instead.
		memory_statistics ret;
		return ret;
	}

	virtual void receive_generic_statistics(generic_statistics & stat)
	{
		
	}
	
	bool sync_to_knowledgebase() {
		(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: syncing to a knowledgebase is not supported by this implementation!\n");
		return false;
	}
	
	bool supports_sync() {
		(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: syncing to a knowledgebase is not supported by this implementation!\n");
		return false;
	}
	
	bool deserialize(serial_stretch & serial) {
		(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: this implementation does not support serialization!\n");
		return false;
	}
	
	basic_string<int32_t> serialize() {
		basic_string<int32_t> ret;
		(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: this implementation does not support serialization!\n");
		return ret;
	}

	bool deserialize_magic(serial_stretch & serial, basic_string<int32_t> & result)
	{
		// expects:
		//	function
		//			0: get_leaf_node_count()
		//			1: get_inner_node_count()
		//			2: set_binary_search()
		//			3: uses_binary_search()
		//	if(function==3)
		//		bool: use_binary_search?
		// returns:
		//	if(function == 0 or 1)
		//		int: value
		//	if(function == 3)
		//		bool

		result.clear();
		if(serial.empty()) return false;
		switch(ntohl(*serial)) {
			case 0:
				cerr << "KV.MAGIC: get_leaf_node_count()\n";
				result += htonl(get_leaf_node_count());
				break;
			case 1:
				cerr << "KV.MAGIC: get_inner_node_count()\n";
				result += htonl(get_inner_node_count());
				break;
			case 2:
				cerr << "KV.MAGIC: set_binary_search()\n";
				++serial;
				if(serial.empty()) return false;
				set_binary_search(ntohl(*serial) != 0);
				break;
			case 3:
				cerr << "KV.MAGIC: uses_binary_search()\n";
				result += htonl(uses_binary_search() ? 1 : 0);
				break;
			default:
				cerr << "KV.MAGIC: unknown command\n";
				return false;
		}
		cerr << "KV.MAGIC: success.\n";
		if(serial.emtpy())
			cerr << "    and used all data.\n";
		else {
			cerr << "    but the following data remains:\n";
			while(!serial.empty()) {
				cerr << ntohl(*serial) << "; "
				++serial;
			}
			cerr << "\n";
		}
		return true;
	}

	/*
	 * Add a counter-example to the algorithm.
	 */
	bool add_counterexample(list<int> counter_example) {
		// If there are tasks to complete, return false
		if(!tasks.is_empty()) {
			(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: no counter-example is expected!\n");
			return false;
		}
		
		/*
		 * Check counter-example
		 */
		// The empty string is provided as counter-example
		if(counter_example.size() == 0) {
			(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: the empty string cannot be a counter-example!\n");
			return false;
		}
		
		// Counter-example is not classified correctly
		answer a;
		if(this->my_knowledge->resolve_query(counter_example, a)) {
			if(!initial_phase) {
				if(a == simulate_run(counter_example)->accepting) {
					(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: invalid counter-example!\n");
					return false;
				}
			}
		}
		
		/*
		 * Initial phase
		 */
		if (initial_phase) {
			
			// Query empty string
			list<int> epsilon;
			answer a;
			if(!this->my_knowledge->resolve_or_add_query(epsilon, a)) {
				(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: need the classification of the empty string!\n");
				return false;
			}
			
			// Create initial tree
			leaf_node *left_child, *right_child;

			if (a == true) {
				left_child = new leaf_node(counter_example, 1, 1, false, this->alphabet_size);
				right_child = new leaf_node(epsilon, 1, 0, true, this->alphabet_size);
				initial_state = right_child;
			} else {
				left_child = new leaf_node(epsilon, 1, 0, false, this->alphabet_size);
				right_child = new leaf_node(counter_example, 1, 1, true, this->alphabet_size);
				initial_state = left_child;
			}
			root = new inner_node(epsilon, 0, left_child, right_child);
			left_child->parent = root;
			right_child->parent = root;
			
			// Add leaf node transitions
			for (int i=0; i<this->alphabet_size; i++) {

					task *t = new compute_transition_task(left_child, i, this);
					if (t->perform()) {
						delete t;
					} else {
						tasks.add_last(t);
					}

					t = new compute_transition_task(right_child, i, this);
					if (t->perform()) {
						delete t;
					} else {
						tasks.add_last(t);
					}
			}
			
			// Set internal variables
			initial_phase = false;
			inner_node_count = 1;
			leaf_node_count = 2;
		}
		
		/*
		 * Normal phase
		 */
		else {
			
			// Create a add_counterexample_task to do the job.
			task *t;
			if(this->use_binary_search)
				t = new add_counterexample_binarysearch_task(counter_example, this);
			else
				t = new add_counterexample_linearsearch_task(counter_example, this);
			
			if (t->perform())
				delete t;
			else 
				tasks.add_last(t);
		}
		
		return true;
	}
	
	virtual bool complete() {
		return false;
	}

	/*
	 * Checks whether a conjecture is ready.
	 */
	bool conjecture_ready() {
		return this->tasks.is_empty();
	}
	
	/*
	 * Computes a conjecture.
	 */
	conjecture * derive_conjecture() {
		
		/*
		 * If we are in the initial phase, let's build an easy automaton.
		 */
		if (initial_phase) {

			// Query empty string
			list<int> epsilon;
			answer a;
			if(!this->my_knowledge->resolve_or_add_query(epsilon, a)) {
				(*this->my_logger)(LOGGER_WARN, "kearns_vazirani: need the classification of the empty string!\n");
				return NULL;
			}
			
			// Create automaton
			simple_automaton *automaton = new simple_automaton;
			automaton->is_deterministic = true;
			automaton->alphabet_size = this->alphabet_size;
			automaton->valid = true;

			automaton->state_count = 1;
			for(int i=0; i<this->alphabet_size; i++) {
				pair<int,int> source (0, i);
				automaton->transitions.insert(pair<pair<int,int>,int>(source, 0));
			}
			
			automaton->initial.insert(0);
			
			if(a == true)
				automaton->final.insert(0);

			return automaton;

		}
		
		/*
		 * Normal phase: Create an automaton from the tree.
		 */
		else {
		
			// Create automaton
			simple_automaton *automaton = new simple_automaton;
			automaton->is_deterministic = true;
			automaton->alphabet_size = this->alphabet_size;
			automaton->valid = true;
			automaton->state_count = 0;
			automaton->initial.insert(0);
		
			// Iterate throug all leaf nodes to generate transitions,
			// beginning with initial_state
			bool visited[this->leaf_node_count];
			for(int i=0; i<this->leaf_node_count; i++)
				visited[i] = false;
			list<leaf_node*> to_process;

			to_process.push_front(initial_state);
			
			while (!to_process.empty()) {
				
				// Get leaf node to process
				leaf_node *current = to_process.front();
				to_process.pop_front();
				visited[current->id] = true;
				automaton->state_count++;
				
				// Add as final state if so
				if(current->accepting)
					automaton->final.insert(current->id);
				
				// Process each transition
				for(int i=0; i<this->alphabet_size; i++) {
					
					pair<int,int> source (current->id, i);
					automaton->transitions.insert(pair<pair<int,int>,int>(source, current->transitions[i]->id));
				
					// If the destination is not yet processed, add it to be processed
					if(!visited[current->transitions[i]->id] && find(to_process.begin(), to_process.end(), current->transitions[i]) == to_process.end())
						to_process.push_back(current->transitions[i]);
				}
				
			}
		
			return automaton;
		}
	}
	
	/*
	 * Prints an internal representation of the learning algorithm including its
	 * tree data structure.
	 */
	void print(ostream &os) {
		
		/*
		 * Are we in the initial phase
		 */
		if(initial_phase) {
			os << "Initial phase (no internal data)." << endl;
			return;
		}
		
		/*
		 * Create dot for the normal phase
		 */
		 
		os << "digraph KV_tree {" << endl << "  fontsize = 8;" << endl << "  rankdir = TD;" << endl;
		
		// Nodes
		map<node*, int> *nodes = new map<node*, int>();

		// Number
		int *id = new int(0);
		
		// 
		map<int, stringstream*> *dot_on_level = new map<int, stringstream*>;
		
		// Dot tree
		map<int, stringstream*>::iterator it;
		dot_tree(root, nodes, id, dot_on_level);
		os << "  {" << endl << "    node [shape=plaintext];" << endl << "    \"root\" ->";
		for(unsigned int i=1; i<dot_on_level->size(); i++) {
			os << "\"level " << i << "\"";
			if(i < dot_on_level->size() - 1)
				os << " -> ";
			else
				os << ";";
		}
		os << endl << "  }" << endl;
		for(unsigned int i=0; i<dot_on_level->size(); i++) {
			os << "  node [shape=box]; {" << endl << "    rank = same;" << endl;
			if(i==0)
				os << "    \"root\";" << endl;
			else
				os << "    \"level " << i << "\";" << endl;
			os << (*dot_on_level)[i]->str();
			os << "  }" << endl;
		}

		dot_transitions(root, nodes, os);

		// Memory cleanup
		for(it = dot_on_level->begin(); it != dot_on_level->end(); it++)
			delete it->second;
		delete dot_on_level;
		delete nodes;
		delete id;
				
		os << "};";
	}
	
	/*
	 * Returns a string representation of the learning algorithm.
	 */
	string to_string() {

		stringstream dot;
		print(dot);
		return dot.str();
	}
	
	private:
	
	/*
	 * Computes the least common ancestor of two nodes in the tree.
	 */
	node *least_commont_ancestor(node *n, node *another_node) {
	
		// First, bring both nodes to the same level: Assume that node is nearer
		// to the top
		if (n->level != another_node->level) {
			if (another_node->level < n->level) {
				node *tmp = n;
				n = another_node;
				another_node = tmp;
			}

			while (another_node->level > n->level) {
				another_node = another_node->parent;
			}
		}

		// Search for common ancestor
		while (n != another_node) {
			n = n->parent;
			another_node = another_node->parent;
		}

		return n;
	}
	
	/*
	 * Simulates the run of the hypothesis on the given input and returns the
	 * reached equivalence class.
	 */
	leaf_node* simulate_run(list<int> &input) {
	
		leaf_node *current = this->initial_state;
		list<int>::iterator it;
	
		// Simulate run
		for(it = input.begin(); it != input.end(); it++)
			current = current->transitions[*it];
		
		return current;
	}
	
	/*
	 * Recursively computes a dot representation of the tree structure.
	 */
	void dot_tree(node *n, map<node*, int> *nodes, int *id, map<int, stringstream*> *dot_on_level) {
	
		/*
		 * Assign a node id
		 */
		nodes->insert(pair<node*,int>(n, (*id)));
		(*id)++;
		
		/*
		 * Check whether there is a node on the current level allready
		 */
		if(dot_on_level->find(n->level) == dot_on_level->end()) {
			stringstream *dot_new_level = new stringstream;
			dot_on_level->insert(pair<int, stringstream*>(n->level, dot_new_level));
		}
		
		/*
		 * Dot current node
		 */
		stringstream *dot = (*dot_on_level)[n->level];
		(*dot) << "    " << (*nodes)[n] << " [";
		if(n == root || n == initial_state)
			(*dot) << "style=\"filled\",";
		(*dot) << "label=\"'";
		list<int>::iterator it;
		for(it=n->label.begin(); it!=n->label.end(); it++)
			(*dot) << (*it) << " ";
		(*dot) << "'";
		// is the current node a leaf node
		if(n->is_leaf()) {
			leaf_node *leaf = dynamic_cast<leaf_node*>(n);
			(*dot) << ", " << leaf->id << "\"";
			(*dot) << ",color=\"";
			if(leaf->accepting)
				(*dot) << "green";
			else
				(*dot) << "red";
			(*dot) << "\"";
		} else {
			(*dot) << "\"";
		}
		(*dot) << "];\n";
		
		/*
         * Recursive calls
		 */
		if(!n->is_leaf()) {
			inner_node *leaf = dynamic_cast<inner_node*>(n);
			dot_tree(leaf->left_child, nodes, id, dot_on_level);
			dot_tree(leaf->right_child, nodes, id, dot_on_level);
		}
	}
	
	/*
	 * Dots the transitions of the hypothesis (i.e. represented by the leaf nodes).
	 */
	void dot_transitions(node *n, map<node*, int> *nodes, ostream &dot) {
		/*
		 * Dot tree connections
		 */
		if(n != root) {
			dot << "  " << (*nodes)[n] << " -> " << (*nodes)[n->parent] << " [style=\"dotted\"];" << endl;
		}
		if (!n->is_leaf()) {
			inner_node *inner = dynamic_cast<inner_node*> (n);
			dot << "  " << (*nodes)[n] << " -> " << (*nodes)[inner->left_child] << " [weight=2, color=\"green\"];" << endl;
			dot << "  " << (*nodes)[n] << " -> " << (*nodes)[inner->right_child] << " [weight=2, color=\"red\"];" << endl;
		}
		
		/*
		 * Dot transitions of hypothesis
		 */
        //Inner node: descent recursively
		if (!n->is_leaf()) {
			inner_node *inner = dynamic_cast<inner_node*> (n);

			dot_transitions(inner->left_child, nodes, dot);
			dot_transitions(inner->right_child, nodes, dot);
		}
		
		// Leaf node: dot transitions of hypothesis
		else {
			leaf_node *leaf = dynamic_cast<leaf_node*> (n);
			
			for (int i=0; i<this->alphabet_size; i++) {
				if(leaf->transitions[i] != NULL) {
					dot << "  " << (*nodes)[n] << " -> " << (*nodes)[leaf->transitions[i]];
					dot << " [fontcolor=\"gray\",label=\"" << i << "\",color=\"gray\"];" << endl;
				}
			}
		}
	}
};

};
