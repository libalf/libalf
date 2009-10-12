/* $Id$
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
 * (c) 2008,2009 by David R. Piegdon, Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_knowledgebase_h__
# define __libalf_knowledgebase_h__

#include <iterator>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <sstream>
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include <queue>
#include <map>

// forward declaration for filter
namespace libalf {
	template <class answer> class knowledgebase;
};

#include <libalf/answer.h>
#include <libalf/alphabet.h>
#include <libalf/filter.h>

// the knowledgebase holds membership information about a language
// (or samples for offline-algorithms). to obtain information about a
// word w, use ::resolve_query(). if the requested information is not
// known, false will be returned. if you wish it to be marked as to be
// acquired (status==NODE_REQUIRED), use ::resolve_or_add_query().
//
// all membership information can be iterated via ::begin() .. ::end().
// all queries via ::qbegin() .. ::qend(). the iterators resolve to a
// ::node. each node represents a word ::node::get_word().
//
// all nodes are arranged in a single tree-like structure, with ::root
// representing epsilon. you can use ::node::parent() or
// ::node::find_child() to find all prefixes of w or all words prefixed
// by w. if ::node::find_child() returns NULL, the concat is not yet
// contained in the tree. using find_or_create_child() solves this by
// adding a new node marked ::node::status==NODE_IGNORE and returning
// it. for this task, you can also use ::add_knowledge().
//
// ::create_query_tree() creates a new knowledgebase containing only the
// queries.
//
// ::merge_knowledgebase() merges membership information (no queries)
// from another knowledgebase into this (e.g. an answered query tree
// created before)

namespace libalf {

using namespace std;

template <class answer>
class knowledgebase {
	public: // types
		class node {
			friend class libalf::knowledgebase<answer>;
			friend class libalf::knowledgebase<answer>::iterator;
			public: // types
				enum status_e {
					NODE_IGNORE = 0,
					NODE_REQUIRED = 1,
					NODE_ANSWERED = 2
				};
			protected: // data
				knowledgebase * base;
				node * parent;
				// for root-node, parent is NULL.
				vector<node *> children;
				// NULL is a valid placeholder for
				// non-existing sub-trees
				int label;
				// as this == parent->children[label], label
				// is redundant. except for root, where label
				// should be -1 == epsilon
				unsigned int timestamp;
				enum status_e status;
				answer ans;
			protected: // internal methods
				node* get_next(node * current_child)
				// used during iterator++
				{{{
					typename vector<node *>::iterator ci;

					// find next after current child
					if(current_child != NULL) {
						ci = find(children.begin(), children.end(), current_child);
						if(ci == children.end())
							ci = children.begin();
						else
							ci++;
					} else {
						ci = children.begin();
					}

					// check children
					for(/* nothing */; ci != children.end(); ci++)
						if(*ci != NULL)
							return *ci;

					// refer to parent
					if(parent)
						return parent->get_next(this);
					else
						return NULL;
				}}}
				void serialize_subtree(basic_string<int32_t> & into)
				// used during serialization
				{{{
					typename vector<node *>::iterator ci;

					into += htonl(label);
					into += htonl(timestamp);
					into += htonl(status);

					if(is_answered())
						into += htonl((int32_t)ans);

					int childcount = 0;
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci != NULL)
							childcount++;
					into += htonl(childcount);

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci != NULL)
							(*ci)->serialize_subtree(into);

				}}}
				bool deserialize_subtree(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit, int & count)
				{{{
					int childcount;

					if(it == limit) return false;

					// skip label, was set by parent
					it++, count--; if(it == limit) return false;
					timestamp = (int)ntohl(*it);
					if(base->timestamp <= timestamp)
						base->timestamp = timestamp + 1;

					it++, count--; if(it == limit) return false;
					status = (enum node::status_e)ntohl(*it);

					if(status == NODE_ANSWERED)
						base->answercount++;

					if(is_answered()) {
						it++, count--; if(it == limit) return false;
						ans = (int32_t)ntohl(*it);
					}

					it++, count--; if(it == limit) return false;
					childcount = ntohl(*it);
					it++, count--;

					for(/* nothing */; childcount > 0; childcount--) {
						node* c;

						if(it == limit) return false;

						c = this->find_or_create_child(ntohl(*it));
						c->deserialize_subtree(it, limit, count);
					}

					return true;
				}}}

			public: // public methods
				node(knowledgebase * base)
				{{{
					this->base = base;
					parent = NULL;
					label = -1;
					status = NODE_IGNORE;
					timestamp = 0;
				}}}
				~node()
				// reference in parent will stay ; all
				// children will be deleted
				{{{
					typename vector<node*>::iterator ci;
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							delete (*ci);
				}}}

				node * get_selfptr()
				{{{
					return this;
				}}}

				node * get_parent()
				{{{
					  return parent;
				}}}
				int max_child_count()
				// if this returns n, there _may_ exist
				// suffixes [0..n)
				{{{
					return children.size();
				}}}
				node * find_child(int label)
				// go to node defined by single-letter suffix
				{{{
					if(label >= 0 && label < (int)children.size())
						return children[label];
					else
						return NULL;
				}}}
				node * find_child(list<int>::iterator infix_start, list<int>::iterator infix_limit)
				// go to node defined by arbitrarily long
				// suffix
				{{{
					node * n = this;

					while(infix_start != infix_limit && n != NULL) {
						n = n->find_child(*infix_start);
						++infix_start;
					}

					return n;
				}}}
				node * find_or_create_child(int label)
				{{{
					if(label < 0)
						return NULL;
					if(label >= (int)children.size()) {
						// push back NULL until new alphabet-size is met
						for(int c = label - children.size(); c >= 0; c--) {
							children.push_back((node *)NULL);
						}
					}

					if(children[label] == NULL) {
						// on demand create new child
						children[label] = new node(base);
						children[label]->parent = this;
						children[label]->label = label;
						base->nodecount++;
					}

					if(label >= base->alphabet_size)
						base->alphabet_size = label+1;

					return children[label];
				}}}
				node * find_or_create_child(list<int>::iterator infix_start, list<int>::iterator infix_limit)
				{{{
					node * n = this;

					while(infix_start != infix_limit && n != NULL) {
						n = n->find_or_create_child(*infix_start);
						++infix_start;
					}

					return n;
				}}}
				bool has_specific_suffix(answer specific_answer)
				// check if a suffix exists with a specific
				// answer
				{{{
					if(is_answered() && get_answer() == specific_answer)
						return true;

					typename vector<node*>::iterator ci;

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							if((*ci)->has_specific_suffix(specific_answer))
								return true;

					return false;
				}}}
				list<int> get_word()
				// get word this node represents
				{{{
					list<int> w;
					node * n = this;

					while(n != NULL) {
						if(n->label >= 0)
							w.push_front(n->label);
						n = n->parent;
					}

					return w;
				}}}
				bool mark_required()
				// returns true if node is now required, false
				// if knowledge is already known.
				{{{
					if(status == NODE_IGNORE) {
						status = NODE_REQUIRED;
						timestamp = base->timestamp;
						base->required.push_back(this);
						return true;
					} else {
						return (status == NODE_REQUIRED);
					}
				}}}
				bool is_required()
				// is this node marked as unknown and
				// required?
				{{{
					return status == NODE_REQUIRED;
				}}}
				bool is_answered()
				{{{
					return status == NODE_ANSWERED;
				}}}
				bool set_answer(answer ans)
				// return false in case of inconsistency (if
				// this was already known and both knowledges
				// differ)
				{{{
					// check for inconsistencies
					if(status == NODE_ANSWERED)
						return ((answer)this->ans == (answer)ans);

					if(status == NODE_REQUIRED)
						base->required.remove(this);

					status = NODE_ANSWERED;
					this->ans = ans;
					this->timestamp = base->timestamp;
					base->timestamp++;

					base->answercount++;

					return true;
				}}}
				answer get_answer()
				{{{
					  return ans;
				}}}
				bool no_subqueries(bool check_self = true)
				// no queries with this node as prefix exist?
				{{{
					if(check_self && status == NODE_REQUIRED)
						return false;

					typename vector<node*>::iterator ci;

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							if( ! (*ci)->no_subqueries(true) )
								return false;

					return true;
				}}}
				bool different(node * other)
				// different answer this and other?
				{{{
					if(is_answered() && other->is_answered()) {
						return (
							(get_answer() == true && other->get_answer() == false)
						     || (get_answer() == false && other->get_answer() == true)
						);
					} else {
						return false;
					}
				}}}
				bool recursive_different(node * other, int depth)
				// two nodes are recursive different if there
				// exists a word w of maximum length `depth'
				// in Sigma* with this.w is different from
				// other.w . to allow infinite long words, use
				// depth = -1.
				{{{
					typename vector<node*>::iterator ci, oci;

					if(different(other))
						return true;

					if(depth == 0)
						return false;

					for(ci = children.begin(), oci = other->children.begin();
					    ci != children.end() && oci != other->children.end();
					    ci++, oci++)   {
						if((*ci) == NULL || (*oci) == NULL)
							continue;
						if((*ci)->recursive_different(*oci), depth > 0 ? depth-1 : -1)
							return true;
					}

					return false;
				}}}
				bool is_prefix_of(node*other)
				// check if this is a suffix of other
				{{{
					while(other != NULL) {
						if(this == other)
							return true;
						other = other->parent;
					}
					return false;
				}}}
				bool is_suffix_of(node*other)
				// check if this is a prefix of other
				{{{
					return other->is_prefix_of(this);
				}}}
				bool is_lex_smaller(node * other)
				// lexicographically compare this to other
				// (true if this < other)
				{{{
					// FIXME: efficience
					list<int> a,b;
					a = this->get_word();
					b = other->get_word();
					return libalf::is_lex_smaller(a,b);
				}}}
				bool is_graded_lex_smaller(node * other)
				// graded lexicographically compare this to
				// other (true if this < other)
				{{{
					// FIXME: efficience
					list<int> a,b;
					a = this->get_word();
					b = other->get_word();
					return libalf::is_graded_lex_smaller(a,b);
				}}}
				unsigned long long int get_memory_usage()
				// calculate memory consumption of this
				// subtree
				{{{
					unsigned long long int ret;
					typename vector<node*>::iterator ci;

					ret = sizeof(this) + sizeof(node *) * children.size();
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							ret += (*ci)->get_memory_usage();

					return ret;
				}}}

				void ignore()
				// set status to ignore (and thus delete
				// answer)
				{{{
					if(status == NODE_REQUIRED)
						base->required.remove(this);
					if(status == NODE_ANSWERED)
						base->answercount--;

					status = NODE_IGNORE;
					timestamp = 0;
				}}}
				bool cleanup()
				// remove all branches that consist only of
				// ignores. returns true if tree is empty (no
				// answers, no queries)
				{{{
					bool may_remove_self;

					may_remove_self = ( this->status == NODE_IGNORE );

					// remove all children that may be removed
					typename vector<node*>::iterator ci;
					for(ci = children.begin(); ci != children.end(); ++ci) {
						if(*ci != NULL) {
							if( ! (*ci)->cleanup() ) {
								may_remove_self = false;
							} else {
								delete *ci;
								*ci = NULL;
							}
						}
					}

					return may_remove_self;
				}}}

		}; // end of knowledgebase::node

		// for efficient node* containers like map and multimap, a
		// comparator object is required.
		class node_comparator {
			public:
				bool operator() (node * a, node * b)
				{ return a < b; };
		};

		// definition of an equivalence relation over nodes in this
		// knowledgebase. you can use
		// knowledgebase::equivalence_relation2automaton to construct
		// a modulo-automaton from this.
		//
		// NOTE: YOU have to take care that this is a true eq.
		// relation, i.e. if you create and maintain it, you have to
		// take care, that the following is fulfilled:
		//
		//	1) reflexivity:		a ~ a
		//
		//	2) symmetry:		a ~ b <=> b ~ a
		//
		//	3) transitivity:	a ~ b && b ~ c => a ~ c
		//
		// nothing (not even #1) is done automatically for you. but
		// all of these rules are required to be fulfilled. otherwise,
		// the functions given here may not work as expected.
		//
		// FIXME: define:
		//	template <order>
		//	class equivalence_class : public set<node*>;
		//	here overload operator[] to give &representative
		//
		//	template <order>
		//	class equivalence_relation : public set<equivalence_class<order> >;
		//	here overlode operator[] to give &equivalence_class.
		class equivalence_relation : public multimap<node*, node*, node_comparator> {
			public: // types
				typedef typename multimap<node*, node*, node_comparator>::iterator iterator;
				typedef pair<iterator, iterator> range;
			public: // member functions
				set<node*> get_equivalence_class(node * n)
				// get all members of equivalence class
				{{{
					set<node*> ret;
					range eq_class;

					eq_class = this->equal_range(n);
					while(eq_class.first != eq_class.second) {
						ret.insert(eq_class.first->second);
						eq_class.first++;
					}
					return ret;
				}}}
				bool are_equivalent(node * a, node * b)
				{{{
					range eq_class;

					eq_class = this->equal_range(a);
					while(eq_class.first != eq_class.second) {
						if(eq_class.first->second == b)
							return true;
						eq_class.first++;
					}
					return false;
				}}}

				node * representative_lex(node * n)
				// get representative of nodes eq.class w.r.t.
				// lexicographic order
				{{{
					range eq_class;
					list<int> current_rep_word;
					node * current_rep;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return NULL;

					current_rep = eq_class.first->second;
					current_rep_word = current_rep->get_word();
					eq_class.first++;

					while(eq_class.first != eq_class.second) {
						list<int> w;
						w = eq_class.first->second->get_word();
						if(libalf::is_lex_smaller(w, current_rep_word)) {
							current_rep = eq_class.first->second;
							current_rep_word = w;
						}
						eq_class.first++;
					}

					return current_rep;
				}}}
				bool is_representative_lex(node * n)
				{{{
					range eq_class;
					list<int> word;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return false;

					word = n->get_word();

					while(eq_class.first != eq_class.second) {
						list<int> w;
						w = eq_class.first->second->get_word();
						if(libalf::is_lex_smaller(w, word))
							return false;
						eq_class.first++;
					}

					return true;
				}}}
				set<node*> representatives_lex()
				// get representatives of all eq.classes
				// w.r.t. lexicographic order
				{{{
					set<node*> ret;
					iterator i;
					node *done = NULL;

					for(i = this->begin(); i != this->end(); i++) {
						if(i->first != done) {
							done = i->first;
							ret.insert(representative_lex(i->first));
						}
					}

					return ret;
				}}}

				node * representative_graded_lex(node * n)
				// get representative of nodes eq.class w.r.t.
				// graded lexicographic order
				{{{
					range eq_class;
					list<int> current_rep_word;
					node * current_rep;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return NULL;

					current_rep = eq_class.first->second;
					current_rep_word = current_rep->get_word();
					eq_class.first++;

					while(eq_class.first != eq_class.second) {
						list<int> w;
						w = eq_class.first->second->get_word();
						if(libalf::is_graded_lex_smaller(w, current_rep_word)) {
							current_rep = eq_class.first->second;
							current_rep_word = w;
						}
						eq_class.first++;
					}

					return current_rep;
				}}}
				bool is_representative_graded_lex(node * n)
				{{{
					range eq_class;
					list<int> word;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return false;

					word = n->get_word();

					while(eq_class.first != eq_class.second) {
						list<int> w;
						w = eq_class.first->second->get_word();
						if(libalf::is_graded_lex_smaller(w, word))
							return false;
						eq_class.first++;
					}

					return true;
				}}}
				set<node*> representatives_graded_lex()
				// get representatives of all eq.classes
				// w.r.t. graded lexicographic order
				{{{
					set<node*> ret;
					iterator i;
					node* done = NULL;

					for(i = this->begin(); i != this->end(); i++) {
						if(i->first != done) {
							done = i->first;
							ret.insert(representative_graded_lex(i->first));
						}
					}

					return ret;
				}}}

				node * representative_ptr(node * n)
				// get representative of nodes eq.class w.r.t.
				// ptr order (very efficient, but arbitrary)
				{{{
					range eq_class;
					node * current_rep;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return NULL;

					current_rep = eq_class.first->second;
					eq_class.first++;

					while(eq_class.first != eq_class.second) {
						if(eq_class.first->second < current_rep)
							current_rep = eq_class.first->second;
						eq_class.first++;
					}

					return current_rep;
				}}}
				bool is_representative_ptr(node * n)
				{{{
					range eq_class;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return false;

					while(eq_class.first != eq_class.second) {
						if(eq_class.first->second < n)
							return false;
						eq_class.first++;
					}

					return true;
				}}}
				set<node*> representatives_ptr()
				// get representatives of all eq.classes
				// w.r.t. ptr order (very efficient, but
				// arbitrary)
				{{{
					set<node*> ret;
					iterator i;
					node* done = NULL;

					for(i = this->begin(); i != this->end(); i++) {
						if(i->first != done) {
							done = i->first;
							ret.insert(representative_ptr(i->first));
						}
					}

					return ret;
				}}}
		}; // end of knowledgebase::equivalence_relation

		// this class can be used to iterate over all nodes markes as known
		// or all nodes marked as required
		class iterator : std::iterator<std::forward_iterator_tag, node> {
			private:
				knowledgebase * base;

				node * current;

				bool queries_only;
				typename list<node*>::iterator qi;
			public:
				iterator()
				{{{
					queries_only = false;
					current = NULL;
					qi = base->required.end();
				}}}
				iterator(const iterator & other)
				{{{
					base = other.base;
					current = other.current;
					queries_only = other.queries_only;
					qi = other.qi;
				}}}

				iterator(bool queries_only, typename list<node*>::iterator currentquery, node * current, knowledgebase * base)
				{{{
					this->base = base;
					this->current = current;
					this->queries_only = queries_only;
					this->qi = currentquery;
				}}}

				iterator & operator++()
				// only valid if queries have not been changed since creation of iterator
				{{{
					if(queries_only) {
						if(qi != base->required.end()) {
							qi++;
							if(qi != base->required.end())
								current = *qi;
							else
								current = NULL;
						} else {
							// nothing
						}
					} else {
						current = current->get_next(NULL);
					}
					return *this;
				}}}
				iterator operator++(int foo)
				{{{
					iterator tmp = (*this);
					operator++();
					return tmp;
				}}}

				bool is_valid()
				{ return current != NULL; }
				node & operator*()
				{{{
					return *current;
				}}}
				node * operator->()
				{{{
					return current;
				}}}
				iterator & operator=(const iterator & it)
				{{{
					base = it.base;
					current = it.current;
					queries_only = it.queries_only;
					qi = it.qi;

					return *this;
				}}}
				bool operator==(const iterator & it)
				{{{
					return (current == it.current);
				}}}
				bool operator!=(const iterator & it)
				{{{
					return (current != it.current);
				}}}

		}; // end of knowledgebase::iterator



		friend class libalf::knowledgebase<answer>::node;
		friend class libalf::knowledgebase<answer>::iterator;
	protected: // data
		// full tree
		node * root;
		// list of all nodes that are required
		list<node *> required;

		// filter that is tried during resolved_queries() and resolve_or_add_query()
		filter<answer> * my_filter;

		int nodecount; // number of nodes in tree
		int answercount; // number of answers stored in this knowledgebase
		// count_queries is required.size().
		int resolved_queries; // number of queries that have been resolved from this knowledgebase

		int alphabet_size;

		unsigned int timestamp;

	public: // methods
		knowledgebase()
		{{{
			root = NULL;
			my_filter = NULL;
			clear();
		}}}

		~knowledgebase()
		{{{
			delete root;
		}}}

		void clear()
		// does not clear stats or filter, only the tree
		{{{
			if(root) // check only required so we dont bang in constructors
				delete root;

			timestamp = 1;
			alphabet_size = 0;

			required.clear();

			root = new node(this);

			nodecount = 1;
			answercount = 0;
			resolved_queries = 0;
		}}}

		void clear_queries()
		// remove all query-marked nodes
		{{{
			while(!required.empty())
				required.front()->ignore();
			root->cleanup();
		}}}

		bool undo(unsigned int count)
		{{{
			iterator it;
//			printf("undo %d with current timestamp %d\n", count, timestamp);
			for(it = this->begin(); it != this->end(); it++)
				if(it->timestamp >= (timestamp - (int)count))
					it->ignore();
			timestamp -= count;
			if(timestamp < 1)
				timestamp = 1;

			root->cleanup();

			return true;
		}}}

		unsigned long long int get_memory_usage()
		{{{
			unsigned long long int ret;

			ret = sizeof(this);
			ret += root->get_memory_usage();
			ret += sizeof(node*) * required.size();

			return ret;
		}}}

		bool is_answered()
		// no information is marked as required?
		{{{
			return (required.size() == 0);
		}}}
		bool is_empty()
		// no information is contained?
		{{{
			return ( (required.size() == 0) && (answercount == 0) );
		}}}

		unsigned int get_timestamp()
		{{{
			return timestamp;
		}}}

		int count_nodes()
		{{{
			return nodecount;
		}}}
		int count_answers()
		{{{
			return answercount;
		}}}
		int count_queries()
		{{{
			return required.size();
		}}}
		int count_resolved_queries()
		{{{
			return resolved_queries;
		}}}
		void reset_resolved_queries()
		{{{
			resolved_queries = 0;
		}}}

		int get_alphabet_size()
		{{{
			return alphabet_size;
		}}}

		void print(ostream &os)
		{{{
			iterator ki;
				list<int> w;

			os << "knowledgebase {\n";

			for(ki = this->begin(); ki != this->end(); ki++) {
				os << "\tnode ";
				w = ki->get_word();
				print_word(os, w);
				os << " marked " << ( (ki->is_answered()) ? "!" : ( (ki->is_required()) ? "?" : "%" ) );
				if(ki->is_answered())
					os << " answered " << ( (ki->get_answer() == true) ? "+" : ( (ki->get_answer() == false) ? "-" : "?" ) );
				os << "\n";
			}

			os << "}\n";
		}}}
		string tostring()
		{{{
			stringstream str;
			this->print(str);
			return str.str();
		}}}
		string generate_dotfile()
		{{{
			string ret;

			char buf[128];
			iterator it;

			list<int> word;
			string wname;

			ret = "digraph knowledgebase {\n"
				"\trankdir=LR;\n"
				"\tsize=8;\n";

			// add all nodes
			for(it = this->begin(); it != this->end(); it++) {
				word = it->get_word();
				wname = word2string(word);
				snprintf(buf, 128, "\tnode [shape=\"%s\", style=\"filled\", color=\"%s\"]; \"%s [%d]\";\n",
						// shape
						it->is_answered() ? "ellipse" : "pentagon",
						// color
						it->is_answered() ? ( (it->get_answer() == true) ? "green" : (it->get_answer() == false ? "red" : "blue"))
								  : (it->is_required() ? "darkorange" : "grey"),
						// name
						wname.c_str(), it->timestamp
					);
				buf[127] = 0;
				ret += buf;
			}
			// and add all connections
			for(it = this->begin(); it != this->end(); it++) {
				typename vector<node *>::iterator ci;
				string toname;

				word = it->get_word();
				wname = word2string(word);
				for(ci = it->children.begin(); ci != it->children.end(); ci++) {
					if(*ci) {
						word = (*ci)->get_word();
						toname = word2string(word);

						snprintf(buf, 128, "\t\"%s [%d]\" -> \"%s [%d]\" [ label = \"%d\" ];\n",
								wname.c_str(), it->timestamp,
								toname.c_str(), (*ci)->timestamp,
								(*ci)->label
							);
						buf[127] = 0;
						ret += buf;
					}
				}
			}

			ret += "}\n";
			return ret;
		}}}
		string generate_dotfile(equivalence_relation & eq)
		{{{
			string ret;

			char buf[128];
			iterator it;

			list<int> word;
			string wname;

			ret = "digraph knowledgebase {\n"
				"\trankdir=LR;\n"
				"\tsize=8;\n";

			// add all nodes
			for(it = this->begin(); it != this->end(); it++) {
				word = it->get_word();
				wname = word2string(word);
				snprintf(buf, 128, "\tnode [shape=\"%s\", style=\"filled\", color=\"%s\"]; \"%s [%d]\";\n",
						// shape
						it->is_answered() ? "ellipse" : "pentagon",
						// color
						it->is_answered() ? ( (it->get_answer() == true) ? "green" : (it->get_answer() == false ? "red" : "blue"))
								  : (it->is_required() ? "darkorange" : "grey"),
						// name
						wname.c_str(), it->timestamp
					);
				buf[127] = 0;
				ret += buf;
			}
			// and add all connections
			for(it = this->begin(); it != this->end(); it++) {
				typename vector<node *>::iterator ci;
				string toname;

				word = it->get_word();
				wname = word2string(word);
				for(ci = it->children.begin(); ci != it->children.end(); ci++) {
					if(*ci) {
						word = (*ci)->get_word();
						toname = word2string(word);

						snprintf(buf, 128, "\t\"%s [%d]\" -> \"%s [%d]\" [ label = \"%d\" ];\n",
								wname.c_str(), it->timestamp,
								toname.c_str(), (*ci)->timestamp,
								(*ci)->label
							);
						buf[127] = 0;
						ret += buf;
					}
				}
			}

			// and mark equivalence relations
			set<node*> done;
			typename equivalence_relation::iterator ni;
			for(ni = eq.begin(); ni != eq.end(); ni++) {
				if(done.find(ni->second) == done.end()) {
					set<node*> eq_class;
					typename set<node*>::iterator si;
					list<node*> sorted_eq_class;

					// sort equivalence class, then draw a line through the class
					eq_class = eq.get_equivalence_class(ni->second);
					int count;
					for(si = eq_class.begin(), count = 0; si != eq_class.end(); si++, count++) {
						done.insert(*si);
						// sorted insert into sorted_eq_class: [FIXME: efficiency is non-existant]
						typename list<node*>::iterator li;
						for(li = sorted_eq_class.begin(); li != sorted_eq_class.end(); li++)
							if( ! (*li)->is_graded_lex_smaller(*si) )
								break;
						sorted_eq_class.insert(li, *si);
					}

					if(count >= 2) {
						typename list<node*>::iterator li1, li2;
						li1 = sorted_eq_class.begin();
						li2 = li1;
						li2++;

						while(li2 != sorted_eq_class.end()) {
							list<int> w1, w2;
							w1 = (*li1)->get_word();
							w2 = (*li2)->get_word();
							snprintf(buf, 128, "\t\"%s [%d]\" -> \"%s [%d]\" [ label=\"\", color=\"orange\", constraint=\"false\", dir=\"none\" ];\n",
									word2string(w1).c_str(), (*li1)->timestamp,
									word2string(w2).c_str(), (*li2)->timestamp);

							ret += buf;
							li1++;
							li2++;
						};
					}
				} else {
				}
			}

			ret += "}\n";
			return ret;
		}}}


		basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;

			ret += 0; // sizeof, will be filled in later

			ret += htonl(resolved_queries);
			root->serialize_subtree(ret);

			ret[0] += htonl(ret.size() - 1);

			return ret;
		}}}
		bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			int size;

			if(it == limit)
				goto deserialization_failed;

			clear();

			size = ntohl(*it);
			it++; if(it == limit) goto deserialization_failed;

			resolved_queries = ntohl(*it);
			if(resolved_queries < 0) goto deserialization_failed;
			it++; size--; if(it == limit) goto deserialization_failed;

			// label of root-node should be -1
			if((int32_t)(ntohl(*it)) != -1)
				goto deserialization_failed;

			if(!root->deserialize_subtree(it, limit, size))
				goto deserialization_failed;

			if(size == 0)
				return true;

		deserialization_failed:
			clear();
			return false;
		}}}
		bool deserialize_query_acceptances(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		// please see create_query_tree() or get_queries() for the expected order
		{{{
			int size;
			iterator ki;

			if(it == limit)
				goto deserialization_failed;

			size = ntohl(*it);
			it++;

			// we expect _exactly_ as much answers as we have required nodes
			if(size != (int)required.size())
				goto deserialization_failed;

			while( ! required.empty() ) {
				ki = this->qbegin();
				if(it == limit)
					goto deserialization_failed;
				answer a;
				a = (int32_t)ntohl(*it);
				ki->set_answer(a);
				size--;
				it++;
			}

			if(size == 0)
				return true;

		deserialization_failed:
			clear();
			return false;
		}}}

		knowledgebase * create_query_tree()
		// the timestamp in the resulting query tree represents
		// the (ascending) order we expect in deserialize_query_acceptances()
		{{{
			knowledgebase * query_tree;
			iterator qi;

			query_tree = new knowledgebase();

			for(qi = this->qbegin(); qi != this->qend(); ++qi) {
				list<int> qw;
				qw = qi->get_word();
				query_tree->add_query(qw, 0);
				query_tree->timestamp++;
			}

			return query_tree;
		}}}
		list<list<int> > get_queries()
		// get list of all queries (in correct order so that deserialize_query_acceptances() may be used)
		{{{
			iterator ki;
			list<list<int> > ret;

			for(ki = this->qbegin(); ki != this->qend(); ++ki)
				ret.push_back(ki->get_word());

			return ret;
		}}}
		bool merge_knowledgebase(knowledgebase & other_tree)
		// only merges answered information, no queries!
		// returns false if knowledge of the trees is inconsistent.
		// all new knowledge will have the same timestamp!
		{{{
			iterator ki;
			int static_timestamp = timestamp; // we want one timestamp for
			// the whole merge, so we have to keep it static!

			for(ki = other_tree.begin(); ki != other_tree.end(); ++ki)
				if(ki->is_answered()) {
					list<int> w;
					w = ki->get_word();
					if(!this->add_knowledge(w, ki->get_answer()))
						return false;
					timestamp = static_timestamp;
				}
			// increment timestamp so we mark one complete merge with a single timestamp
			timestamp++;
			return true;
		}}}

		bool add_knowledge(list<int> & word, answer acceptance)
		// will return false if knowledge for this word was
		// already set and is != acceptance. in this case, the
		// holder is in an inconsistent state and the
		// knowledgebase will not change itself.
		{{{
			return root->find_or_create_child(word.begin(), word.end())->set_answer(acceptance);
		}}}
		int add_query(list<int> & word, int prefix_count = 0)
		// returns the number of new required nodes (excluding
		// those already known.
		{{{
			node * current;
			list<int>::iterator wi;
			int skip_prefixes;
			int new_queries = 0;

			skip_prefixes = word.size() - prefix_count;
			current = root;

			// walk path to target and mark all required prefixes
			for(wi = word.begin(); wi != word.end(); wi++, skip_prefixes--) {
				if(skip_prefixes <= 0)
					if(current->mark_required())
						new_queries++;
				current = current->find_or_create_child(*wi);
			}

			// mark target itself
			if(current->mark_required())
				new_queries++;

			return new_queries;
		}}}
		bool resolve_query(list<int> & word, answer & acceptance)
		// returns true if known
		// will also try to apply the filter, if set.
		// if the filter knows the answer, the answer will not
		// be stored in the knowledgebase but returned to the user.
		{{{
			node * current;

			current = root->find_child(word.begin(), word.end());

			if(current != NULL && current->is_answered()) {
				acceptance = current->get_answer();
				resolved_queries++;
				return true;
			} else {
				if(my_filter)
					return my_filter->evaluate(*this, word, acceptance);
				else
					return false;
			}
		}}}
		bool resolve_or_add_query(list<int> & word, answer & acceptance)
		// returns true if known. otherwise marks knowledge as
		// to-be-acquired and returns false.
		// will also try to apply the filter, if set.
		// if the filter knows the answer, it will be stored into the
		// knowledgebase.
		{{{
			node * current;

			current = root->find_or_create_child(word.begin(), word.end());

			if(current->is_answered()) {
				acceptance = current->get_answer();
				resolved_queries++;
				return true;
			} else {
				if(my_filter && my_filter->evaluate(*this, word, acceptance)) {
					current->set_answer(acceptance);
					return true;
				}

				current->mark_required();
				return false;
			}
		}}}
		node* get_nodeptr(list<int> & word)
		// get node* for a specific word
		{{{
			return root->find_or_create_child(word.begin(), word.end());
		}}}
		node* get_rootptr()
		// get node* for epsilon
		{{{
			return root;
		}}}

		iterator begin()
		// begin() always begins at root node (epsilon)!
		{{{
			iterator it(false, required.end(), root, this);
			return it;
		}}}
		iterator end()
		{{{
			iterator it;
			return it;
		}}}

		iterator qbegin()
		// get begin-iterator for a query-iterator
		{{{
			if(required.empty()) {
				return qend();
			} else {
				iterator it(true, required.begin(), required.front(), this);
				return it;
			}
		}}}
		iterator qend()
		{{{
			iterator it;
			return it;
		}}}

		bool equivalence_relation2automaton(equivalence_relation &eq, bool ignoring_states_accept,
			bool &t_is_dfa, int &t_alphabet_size, int &t_state_count, set<int> &t_initial,
			set<int> &t_final, multimap<pair<int, int>, int> &t_transitions)
		// construct a modulo automaton given the tree-structure
		// in this knowledgebase and the equivalence relation.
		// will return false if the equivalence relation is
		// inconsistent, i.e. it merges states that are answered
		// with different knowledge.
		{{{
			// get a set of all representatives
			set<node*> representatives;
			typename set<node*>::iterator si;
			map<node*, int> mapping;
			int i;

			// map representatives to state-IDs
			representatives = eq.representatives_ptr();
			for(i=0, si = representatives.begin(); si != representatives.end(); i++, si++)
				mapping[*si] = i;

			t_alphabet_size = -1; // will be adjusted on the fly
			t_is_dfa = false;
			t_state_count = representatives.size();
			t_initial.clear();
			t_initial.insert(mapping[eq.representative_ptr(get_rootptr())]);
			t_final.clear();
			t_transitions.clear();

			// get final states and transitions
			vector<extended_bool> acceptances;
			extended_bool b;
			b.value = extended_bool::EBOOL_UNKNOWN;
			for(i = 0; i < t_state_count; i++)
				acceptances.push_back(b);

			typename equivalence_relation::iterator eqi;
			node *n = NULL;
			for(eqi = eq.begin(); eqi != eq.end(); eqi++) {
				if(eqi->first != n) {
					n = eqi->first;
					int childcount = n->max_child_count();

					if(t_alphabet_size < childcount)
						t_alphabet_size = childcount;

					pair<int, int> trid;
					trid.first = mapping[eq.representative_ptr(n)];
					for(i = 0; i < childcount; i++) {
						// add transition if exists
						node *c;
						c = n->find_child(i);
						if(c != NULL) {
							trid.second = i;
							t_transitions.insert( pair<pair<int, int>, int>(trid, mapping[eq.representative_ptr(c)]));
						}
					}
					if(n->is_answered()) {
						if(n->get_answer() == true) {
							if(acceptances[trid.first].value == extended_bool::EBOOL_FALSE) {
								list<int> word;
								word = n->get_word();
//								printf("inconsistency in equivalence relation: %s\n", word2string(word).c_str());
								return false;
							}
							acceptances[trid.first].value = extended_bool::EBOOL_TRUE;
						} else {
							if(n->get_answer() == false) {
								if(acceptances[trid.first].value == extended_bool::EBOOL_TRUE) {
									list<int> word;
									word = n->get_word();
//									printf("inconsistency in equivalence relation: %s\n", word2string(word).c_str());
									return false;
								}
								acceptances[trid.first].value = extended_bool::EBOOL_FALSE;
							} else {
								// well what can we do :)
							}
						}
					}
				}
			}

			for(i = 0; i < t_state_count; i++) {
				if(acceptances[i].value == extended_bool::EBOOL_TRUE)
					t_final.insert(i);
				else
					if(acceptances[i].value == extended_bool::EBOOL_UNKNOWN && ignoring_states_accept)
						t_final.insert(i);
			}

			return true;
		}}}

}; // end of knowledgebase

// classes to iterate over a full subtree, in graded lexicographic order:
// PURE FORWARD ITERATOR

// iterate in graded lex. order:
// a < b  iff  |a| < |b| or |a|==|b| && (a <[LEX] b)
template <class answer>
class kIterator_lex_graded {
	private:
		queue<typename knowledgebase<answer>::node*> pending;
	public:
		kIterator_lex_graded(typename knowledgebase<answer>::node* root)
		{ set_root(root); }
		~kIterator_lex_graded()
		{ };

		void set_root(typename knowledgebase<answer>::node* root)
		{{{
			while(!pending.empty())
				pending.pop();
			pending.push(root);
		}}}

		kIterator_lex_graded & operator++()
		{{{
			typename vector<typename knowledgebase<answer>::node*>::iterator ci;
			typename knowledgebase<answer>::node* n;

			if(pending.empty()) {
				return *this;
			} else {
				n = pending.front();
				pending.pop();
				for(int sigma = 0; sigma < n->max_child_count(); sigma++) {
					typename knowledgebase<answer>::node * c;
					c = n->find_child(sigma);
					if(c)
						pending.push(c);
				}
			}

			return *this;
		}}}
		kIterator_lex_graded operator++(int foo)
		{{{
			kIterator_lex_graded tmp = (*this);
			operator++();
			return tmp;
		}}}
		typename knowledgebase<answer>::node & operator*()
		{{{
			return *(pending.front());
		}}}
		typename knowledgebase<answer>::node * operator->()
		{{{
			return pending.front();
		}}}

		bool end()
		{ return pending.empty(); };

		bool operator==(const kIterator_lex_graded & it)
		{ return (*this == *it); }
		bool operator!=(const kIterator_lex_graded & it)
		{ return (*this != *it); }

		kIterator_lex_graded & operator=(const kIterator_lex_graded & it)
		{{{
			pending = it.pending;
			return *this;
		}}}
}; // end of kIterator_lex_graded

}; // end of namespace libalf

#endif // __libalf_knowledgebase_h__

