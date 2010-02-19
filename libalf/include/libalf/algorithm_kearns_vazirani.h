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
#include <typeinfo> 
#include <string>
#include <sstream>
#include <stdlib.h>
 
#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

template <class answer>
class kearns_vazirani : public learning_algorithm<answer> {
	friend class task;

	private:
	
	/*
	 * Definition of nodes
	 */
	
	/*
	 * Basic class of a node.
	 */
	class node {
		public:
		list<int> label;
		node *parent;
		int level;
		
		node (list<int> label, int level) {
			this->label = label;
			this->level = level;
		}
		
		virtual bool is_leaf() = 0;
	};

	/*
	 * An inner node
	 */ 
	class inner_node : public node {
		public:
		node *left_child, *right_child;
		
		inner_node (list<int> label, int level, node *left_child, node *right_child)
		: node (label, level) {
			this->left_child = left_child;
			this->right_child = right_child;
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
		bool accepting;
		set<leaf_node*> incoming_transitions;
		leaf_node** transitions;
		int id;
		
		leaf_node (list<int> label, int level, int id, bool accepting, int alphabet_size)
		: node (label, level) {
			this->id = id;
			this->accepting = accepting;
			this->transitions = new leaf_node*[alphabet_size];
			for(int i=0; i<alphabet_size; i++)
				this->transitions[i] = NULL;
		}
		
		bool is_leaf() {
			return true;
		}
	};
	
	/*
	 * Definition of tasks
	 */
	
	/*
	 * Root class of all tasks
	 */
	class task {
		public:
		task *prev, *next;
		
		virtual bool perform() = 0;
		virtual string to_string() = 0;
	};
	
	/*
	 * Compute transition task
	 */
	class compute_transition_task : public task {
		private:
		leaf_node *source;
		node *current_node;
		int symbol;
		kearns_vazirani *kv;

		public:
		compute_transition_task(leaf_node *source, int symbol, kearns_vazirani *kv) {
			this->source = source;
			this->symbol = symbol;
			this->kv = kv;
			current_node = kv->root;
		}
		
		bool perform() {

			list<int> transition_label (source->label.begin(), source->label.end());
			transition_label.push_back(symbol);
		
			// Sift the transition down the tree
			do {

				list<int> query(transition_label.begin(), transition_label.end());
				query.insert(query.end(), current_node->label.begin(), current_node->label.end());
				answer a;
				if(!kv->my_knowledge->resolve_or_add_query(query, a))
					return false;
				
				// DEGUB =====
				
				
				
				// DEGUB END =====
				
				inner_node *inner = dynamic_cast<inner_node*> (current_node);
				
				if(a == true)
					current_node = inner->right_child;
				else
					current_node = inner->left_child;
				
			} while (!current_node->is_leaf());
			
			// Set transition
			leaf_node *leaf;
			leaf = dynamic_cast<leaf_node*> (current_node);
			source->transitions[symbol] = leaf;
			leaf->incoming_transitions.insert(source);
			
			return true;
		}
		
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
	 * Add counter-example task
	 */
	class add_counterexample_simple_task : public task {
		private:
		list<int> counterexample;
		list<int> *prefix;
		node *sift_node;
		kearns_vazirani *kv;
		int position;

		public:
		add_counterexample_simple_task(list<int> counterexample, kearns_vazirani *kv) {
			this->counterexample = counterexample;
			this->sift_node = kv->root;
			this->kv = kv;
			position = 1;
			prefix = NULL;
			new_prefix();
		}
		
		private:
		void new_prefix() {
		
			delete this->prefix;
			
			this->prefix = new list<int>;
			list<int>::iterator it;
			it = counterexample.begin();
			for(int i=1; i<=position; i++) {
				prefix->push_back(*it);
				it++;
			}
			
		}
		
		public:
		bool perform() {
			do {
				
				// If we have finished sifting, we need to check another prefix
				if (sift_node->is_leaf()) {
					sift_node = kv->root;
					position++;
					new_prefix();
				}

				// Get the leaf node that represents the equivalence class of the prefix
				do {
					
					// Perform membership query
					list<int> query(prefix->begin(), prefix->end());
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
				
				// Check for Trennpunkt
				if (run_node != sift_node) {

					// Get next to last state of the run
					list<int> next_to_last;
					list<int>::iterator it;
					it = counterexample.begin();
					for(int i=1; i<=position-1; i++) {
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

					return true;
				}
			
			} while (position < counterexample.size());
			
			cout << "Error: Found no Trennpunk!" << endl;
			return true;
		}
		
		string to_string() {
			stringstream descr;
			descr << "Add counter-example task (counter example: \"";
			
			list<int>::iterator it;
			for(it = counterexample.begin(); it != counterexample.end(); it++)
				descr << (*it) << " ";
			descr << "\")";
			
			return descr.str();
		}
	};
		
	/*
	 * split node task
	 */
	class split_node_task : public task {
		private:
		leaf_node *node_to_split;
		list<int> new_leaf_node_label, new_inner_node_label;
		kearns_vazirani *kv;

		public:
		split_node_task(leaf_node *node_to_split, list<int> &new_leaf_node_label, list<int> &new_inner_node_label, kearns_vazirani *kv) {
			this->node_to_split = node_to_split;
			this->new_leaf_node_label = new_leaf_node_label;
			this->new_inner_node_label = new_inner_node_label;
			this->kv = kv;
		}

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
	
	
	/*
	 * A list of tasks
	 */
	class task_list {
		// First and last tasks
		private:
		task *first, *last;
		
		public:
		task_list () {
			first = last = NULL;
		}

		bool is_empty() {
			return first == NULL;
		}
		
		task *get_first() {
			return first;
		}
		
		int size() {
			int i=0;
			task *t = first;
			while(t != NULL) {
				t = t->next;
				i++;
			}
			
			return i;
		}
		
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
	
	/*
	 * Variables
	 */
	
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
	
	public:
	
	kearns_vazirani (knowledgebase<answer> *base, logger *log, int alphabet_size) {
		// First store the parameters
		this->set_alphabet_size(alphabet_size);
		this->set_logger(log);
		this->set_knowledge_source(base);
		
		// Initial variables
		initial_phase = true;
		
		// Query the empty word
		list<int> epsilon;
		answer a;
		this->my_knowledge->resolve_or_add_query(epsilon, a);
	}
	
	list<int> concatenate(list<int> &first, list<int> &second) {
		list<int> concat (first.begin(), first.end());
		concat.insert(concat.end(), second.begin(), second.end());

		return concat;
	}
	
	/*
	 * Methods
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
	}
	
	memory_statistics get_memory_statistics() {
		memory_statistics ret;
		return ret;
	}
	
	bool sync_to_knowledgebase() {
		return false;
	}
	
	bool supports_sync() {
		return false;
	}
	
	bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) {
		return false;
	}
	
	basic_string<int32_t> serialize() {
		return NULL;
	}
 
	bool add_counterexample(list<int> counter_example) {
		// If there are tasks to complete, return false
		if(!tasks.is_empty()) {
			return false;
		}
		
		// Check counter-example
				
		/*
		 * Initial phase
		 */
		if (initial_phase) {
			
			// Query empty string
			list<int> epsilon;
			answer a;
			if(!this->my_knowledge->resolve_or_add_query(epsilon, a)) {
				cout << "Error: should have the classification of the empty string!" << endl;
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
			
			add_counterexample_simple_task *t = new add_counterexample_simple_task(counter_example, this);
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

	bool conjecture_ready() {
		return false;
	}
	
	conjecture * derive_conjecture() {
		
		/*
		 * If we are in the initial phase, let's build an easy automaton.
		 */
		if (initial_phase) {

			// Query empty string
			list<int> epsilon;
			answer a;
			if(!this->my_knowledge->resolve_or_add_query(epsilon, a)) {
				// cout << "Error: should have the classification of the empty string!" << endl;
				return NULL;
			}
			
			// Create automaton
			simple_automaton *automaton = new simple_automaton;
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
			automaton->alphabet_size = this->alphabet_size;
			automaton->valid = true;
			automaton->state_count = this->leaf_node_count;
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
				
				// Add as final state if so
				if(current->accepting)
					automaton->final.insert(current->id);
				
				// Process each transition
				for(int i=0; i<this->alphabet_size; i++) {
					
					pair<int,int> source (current->id, i);
					automaton->transitions.insert(pair<pair<int,int>,int>(source, current->transitions[i]->id));
				
					// If the destination is not yet processed, add it to be processed
					if(!visited[current->transitions[i]->id])
						to_process.push_back(current->transitions[i]);
				}
				
			}

			return automaton;
		}
	}
	
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
	
	leaf_node* simulate_run(list<int> &input) {
	
		leaf_node *current = this->initial_state;
		list<int>::iterator it;
	
		// Simulate run
		for(it = input.begin(); it != input.end(); it++)
			current = current->transitions[*it];
		
		return current;
	}
	
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
		for(int i=1; i<dot_on_level->size(); i++) {
			os << "\"level " << i << "\"";
			if(i < dot_on_level->size() - 1)
				os << " -> ";
			else
				os << ";";
		}
		os << endl << "  }" << endl;
		for(int i=0; i<dot_on_level->size(); i++) {
			//// cout << "i=" << i
			os << "  node [shape=box]; {" << endl << "    rank = same;" << endl;
			if(i==0)
				os << "    \"root\";" << endl;
			else
				os << "    \"level " << i << "\";" << endl;
			os << (*dot_on_level)[i]->str();
			os << "  }" << endl;
		}

		dot_transitions(root, nodes, os);

		delete nodes;
		delete dot_on_level;
		
		os << "};";
	}
	
	string tostring() {

		stringstream dot;
		print(dot);
		return dot.str();
	}
	
	private:
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