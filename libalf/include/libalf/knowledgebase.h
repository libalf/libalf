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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
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
#include <algorithm>
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include <queue>
#include <map>
#include <typeinfo>

// forward declaration for filter
namespace libalf {
	template <class answer> class knowledgebase;
};

#include <libalf/answer.h>
#include <libalf/alphabet.h>
#include <libalf/filter.h>
#include <libalf/conjecture.h>

#include <libalf/serialize.h>

// the knowledgebase holds membership information about a language
// (or samples for offline-algorithms). to obtain information about a
// word w, use kb::resolve_query(). if the requested information is not
// known, false will be returned. if you wish it to be marked as to be
// acquired (status==NODE_REQUIRED), use kb::resolve_or_add_query().
//
// all membership information can be iterated via kb::begin() .. kb::end().
// all queries via kb::qbegin() .. kb::qend(). the iterators resolve to a
// kb::node. each node represents a word kb::nodekb::get_word().
//
// all nodes are arranged in a single tree-like structure, with kb::root
// representing epsilon. you can use kb::nodekb::parent() or
// kb::nodekb::find_child() to find all prefixes of w or all words prefixed
// by w. if kb::nodekb::find_child() returns NULL, the concat is not yet
// contained in the tree. using find_or_create_child() solves this by
// adding a new node marked kb::nodekb::status==NODE_IGNORE and returning
// it. for this task, you can also use kb::add_knowledge().
//
// kb::create_query_tree() creates a new knowledgebase containing only the
// queries.
//
// kb::merge_knowledgebase() merges membership information (no queries)
// from another knowledgebase into this (e.g. an answered query tree
// created before)

namespace libalf {

template <class answer>
class knowledgebase {
	public: // types
		class node {
			friend class knowledgebase<answer>;
			friend class knowledgebase<answer>::iterator;
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
				std::vector<node *> children;
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
				// used in iterator++
				{{{
					typename std::vector<node *>::iterator ci;

					// find next after current child
					if(current_child != NULL) {
						ci = std::find(children.begin(), children.end(), current_child);
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
				void serialize_subtree(std::basic_string<int32_t> & into) const
				// used during serialization
				{{{
					typename std::vector<node *>::const_iterator ci;

					into += ::serialize(label); // label MUST be the first element (see deserialize of children)
					into += ::serialize(timestamp);
					into += ::serialize(status);

					if(is_answered())
						into += ::serialize(ans);

					int childcount = 0;
					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci != NULL)
							childcount++;
					into += ::serialize(childcount);

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci != NULL)
							(*ci)->serialize_subtree(into);

				}}}
				bool deserialize_subtree(serial_stretch & ser)
				{{{
					using libalf::deserialize;
					if(!::deserialize(label, ser)) return false;
					if(!::deserialize(timestamp, ser)) return false;
					if(timestamp >= base->timestamp)
						base->timestamp = timestamp + 1;
					int st;
					if(!::deserialize(st, ser)) return false;
					status = (enum status_e) st;

					if(is_answered()) {
						if(!deserialize(this->ans, ser)) return false;
						base->answercount += 1;
					}

					int childcount;
					if(!::deserialize(childcount, ser)) return false;

					while(childcount) {
						if(ser.empty()) return false;
						node * c = this->find_or_create_child(ntohl(*ser));
						if(!c->deserialize_subtree(ser)) return false;
						--childcount;
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
					typename std::vector<node*>::iterator ci;
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
				int max_child_count() const
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
				node * find_descendant(std::list<int>::const_iterator infix_start, std::list<int>::const_iterator infix_limit)
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

					if(label >= base->largest_symbol)
						base->largest_symbol = label+1;

					return children[label];
				}}}
				node * find_or_create_descendant(std::list<int>::iterator infix_start, std::list<int>::iterator infix_limit)
				{{{
					node * n = this;

					while(infix_start != infix_limit && n != NULL) {
						n = n->find_or_create_child(*infix_start);
						++infix_start;
					}

					return n;
				}}}
				bool has_specific_suffix(answer specific_answer) const
				// check if a suffix exists with a specific
				// answer
				{{{
					if(is_answered() && get_answer() == specific_answer)
						return true;

					typename std::vector<node*>::const_iterator ci;

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							if((*ci)->has_specific_suffix(specific_answer))
								return true;

					return false;
				}}}

				int get_label() const
				// get label of this node (i.e. last letter in word this node represents)
				{{{
					return this->label;
				}}}
				std::list<int> get_word() const
				// get word this node represents
				{{{
					std::list<int> w;
					const node * n = this;

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
				bool is_required() const
				// is this node marked as unknown and
				// required?
				{{{
					return status == NODE_REQUIRED;
				}}}
				bool is_answered() const
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
				answer get_answer() const
				{{{
					  return ans;
				}}}
				bool no_subqueries(bool check_self = true) const
				// no queries with this node as prefix exist?
				{{{
					if(check_self && status == NODE_REQUIRED)
						return false;

					typename std::vector<node*>::const_iterator ci;

					for(ci = children.begin(); ci != children.end(); ci++)
						if(*ci)
							if( ! (*ci)->no_subqueries(true) )
								return false;

					return true;
				}}}
				bool different(const node * other) const
				// different answer this and other?
				{{{
					if(is_answered() && other->is_answered()) {
						return ( this->get_answer() != other->get_answer() );
					} else {
						return false;
					}
				}}}
				bool recursive_different(const node * other, int depth) const
				// two nodes are recursive different if there
				// exists a word w of maximum length `depth'
				// in Sigma* with this.w is different from
				// other.w . to allow infinite long words, use
				// depth = -1.
				{{{
					typename std::vector<node*>::const_iterator ci, oci;

					if(different(other))
						return true;

					if(depth == 0)
						return false;

					for(ci = children.begin(), oci = other->children.begin();
					    ci != children.end() && oci != other->children.end();
					    ci++, oci++)   {
						if((*ci) == NULL || (*oci) == NULL)
							continue;
						if((*ci)->recursive_different(*oci, depth > 0 ? depth-1 : -1))
							return true;
					}

					return false;
				}}}
				bool is_prefix_of(const node *other) const
				// check if this is a suffix of other
				{{{
					while(other != NULL) {
						if(this == other)
							return true;
						other = other->parent;
					}
					return false;
				}}}
				bool is_suffix_of(const node *other)
				// check if this is a prefix of other
				{{{
					return other->is_prefix_of(this);
				}}}
				bool is_lex_smaller(const node *other) const
				// lexicographically compare this to other
				// (true if this < other)
				{{{
					// FIXME: efficiency
					using libalf::is_lex_smaller;

					std::list<int> a,b;
					a = this->get_word();
					b = other->get_word();
					return is_lex_smaller(a,b);
				}}}
				bool is_graded_lex_smaller(const node *other) const
				// graded lexicographically compare this to
				// other (true if this < other)
				{{{
					// FIXME: efficiency
					using libalf::is_graded_lex_smaller;

					std::list<int> a,b;
					a = this->get_word();
					b = other->get_word();
					return is_graded_lex_smaller(a,b);
				}}}
				unsigned long long int get_memory_usage() const
				// calculate memory consumption of this
				// subtree
				{{{
					unsigned long long int ret;
					typename std::vector<node*>::const_iterator ci;

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
					typename std::vector<node*>::iterator ci;
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
				bool operator() (node * a, node * b) const
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
		//	class equivalence_class : public std::set<node*>;
		//	here overload operator[] to give &representative
		//
		//	template <order>
		//	class equivalence_relation : public std::set<equivalence_class<order> >;
		//	here overlode operator[] to give &equivalence_class.
		class equivalence_relation : public std::multimap<node*, node*, node_comparator> {
			public: // types
				typedef typename std::multimap<node*, node*, node_comparator>::iterator iterator;
				typedef std::pair<iterator, iterator> range;
			public: // member functions
				std::set<node*> get_equivalence_class(node * n)
				// get all members of equivalence class
				{{{
					std::set<node*> ret;
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
					using libalf::is_lex_smaller;

					range eq_class;
					std::list<int> current_rep_word;
					node * current_rep;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return NULL;

					current_rep = eq_class.first->second;
					current_rep_word = current_rep->get_word();
					eq_class.first++;

					while(eq_class.first != eq_class.second) {
						std::list<int> w;
						w = eq_class.first->second->get_word();
						if(is_lex_smaller(w, current_rep_word)) {
							current_rep = eq_class.first->second;
							current_rep_word = w;
						}
						eq_class.first++;
					}

					return current_rep;
				}}}
				bool is_representative_lex(node * n)
				{{{
					using libalf::is_lex_smaller;

					range eq_class;
					std::list<int> word;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return false;

					word = n->get_word();

					while(eq_class.first != eq_class.second) {
						std::list<int> w;
						w = eq_class.first->second->get_word();
						if(is_lex_smaller(w, word))
							return false;
						eq_class.first++;
					}

					return true;
				}}}
				std::set<node*> representatives_lex()
				// get representatives of all eq.classes
				// w.r.t. lexicographic order
				{{{
					std::set<node*> ret;
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
					using libalf::is_graded_lex_smaller;

					range eq_class;
					std::list<int> current_rep_word;
					node * current_rep;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return NULL;

					current_rep = eq_class.first->second;
					current_rep_word = current_rep->get_word();
					eq_class.first++;

					while(eq_class.first != eq_class.second) {
						std::list<int> w;
						w = eq_class.first->second->get_word();
						if(is_graded_lex_smaller(w, current_rep_word)) {
							current_rep = eq_class.first->second;
							current_rep_word = w;
						}
						eq_class.first++;
					}

					return current_rep;
				}}}
				bool is_representative_graded_lex(node * n)
				{{{
					using libalf::is_graded_lex_smaller;

					range eq_class;
					std::list<int> word;

					eq_class = this->equal_range(n);
					if(eq_class.first == eq_class.second)
						return false;

					word = n->get_word();

					while(eq_class.first != eq_class.second) {
						std::list<int> w;
						w = eq_class.first->second->get_word();
						if(is_graded_lex_smaller(w, word))
							return false;
						eq_class.first++;
					}

					return true;
				}}}
				std::set<node*> representatives_graded_lex()
				// get representatives of all eq.classes
				// w.r.t. graded lexicographic order
				{{{
					std::set<node*> ret;
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
				std::set<node*> representatives_ptr()
				// get representatives of all eq.classes
				// w.r.t. ptr order (very efficient, but
				// arbitrary)
				{{{
					std::set<node*> ret;
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

	/*
	 * future replacement of equivalence_relation:
	 *
		class eq_class : public std::set<int> {
			public: // types
				typedef typename std::set<node*>::iterator iterator;
				typedef typename std::set<node*>::const_iterator const_iterator;
			public: // methods
				bool contains(const node * element) const
				{ return (this->find(element) != this->end()); }
				void print(std::ostream &os) const
				{{{
					const_iterator i;

					os << "{ ";
					i = this->begin();
					while(i != this->end()) {
						os << *i;
						i++;
						if(i != this->end())
							os << ", ";
					}
					os << " }";
				}}}
				std::string to_string() const
				{{{
					std::stringstream str;
					this->print(str);
					return str.str();
				}}}
		};

		class eq_relation : public std::set<eq_class> {
			public: // types
				typedef typename std::set<eq_class>::iterator iterator;
				typedef typename std::set<eq_class>::const_iterator const_iterator;
			public: // methods
				// merge two equivalence classes (Forced without any check)
				void immediate_Fmerge(iterator i, iterator j)
				{{{
					if(i->size() < j->size()) {
						merge(j, i); // thats faster
					} else {
						// insert j into i
						this->insert(j->begin(), j->end());

						// delete j
						this->erase(j);
					}
				}}}
				// merge two equivalence classes and all of their children accordingly.
				// (Forced without any check)
				bool successive_Fmerge(iterator i, iterator j)
				{

				}
				iterator find_class_of(node * element)
				{{{
					iterator i;
					for(i = this->begin(); i != this->end(); ++i)
						if(i->contains(element))
							break;
					return i;
				}}}
				const_iterator find_class_of(const node * element)
				{{{
					const_iterator i;
					for(i = this->begin(); i != this->end(); ++i)
						if(i->contains(element))
							break;
					return i;
				}}}
				void print(std::ostream &os) const
				{{{
					const_iterator i;

					os << "{ ";
					i = this->begin();
					while(i != this->end()) {
						i->print(os);
						i++;
						if(i != this->end())
							os << ", ";
					}
					os << " }";
				}}}
				std::string to_string() const
				{{{
					std::stringstream str;
					this->print(str);
					return str.str();
				}}}
		};
	*/

		// this class can be used to iterate over all nodes markes as known
		// or all nodes marked as required
		class iterator : std::iterator<std::forward_iterator_tag, node> {
			private:
				knowledgebase * base;

				node * current;

				bool queries_only;
				typename std::list<node*>::iterator qi;
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

				iterator(bool queries_only, typename std::list<node*>::iterator currentquery, node * current, knowledgebase * base)
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
				iterator operator++(int __attribute__ ((__unused__)) foo)
				{{{
					iterator tmp = (*this);
					operator++();
					return tmp;
				}}}

				bool is_valid() const
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
				bool operator==(const iterator & it) const
				{{{
					return (current == it.current);
				}}}
				bool operator!=(const iterator & it) const
				{{{
					return (current != it.current);
				}}}
		}; // end of knowledgebase::iterator



		friend class knowledgebase<answer>::node;
		friend class knowledgebase<answer>::iterator;
	protected: // data
		// full tree
		node * root;
		// list of all nodes that are required
		std::list<node *> required;

		// filter that is tried during resolved_queries() and resolve_or_add_query()
		filter<answer> * my_filter;

		int nodecount; // number of nodes in tree
		int answercount; // number of answers stored in this knowledgebase
		// count_queries is required.size().
		int resolved_queries; // number of queries that have been resolved from this knowledgebase

		int largest_symbol; // largest symbol that ever was stored in knowledgebase.
			// usually this is only increased. only manually via check_largest_symbol()
			// you can enforce a check of the complete knowledgebase.

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
			largest_symbol = 0;

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

		unsigned long long int get_memory_usage() const
		{{{
			unsigned long long int ret;

			ret = sizeof(this);
			ret += root->get_memory_usage();
			ret += sizeof(node*) * required.size();

			return ret;
		}}}

		bool is_answered() const
		// no information is marked as required?
		{{{
			return (required.size() == 0);
		}}}
		bool is_empty() const
		// no information is contained?
		{{{
			return ( (required.size() == 0) && (answercount == 0) );
		}}}

		unsigned int get_timestamp() const
		{{{
			return timestamp;
		}}}

		int count_nodes() const // O(1)
		{{{
			return nodecount;
		}}}
		int count_answers() const // O(1)
		{{{
			return answercount;
		}}}
		int count_queries() const // O(n) !
		{{{
			return required.size();
		}}}
		int count_resolved_queries() const // O(1)
		{{{
			return resolved_queries;
		}}}
		void reset_resolved_queries() const
		{{{
			resolved_queries = 0;
		}}}

		int get_largest_symbol() const
		// return largest known symbol ( O(1) ) that was ever
		// stored in knowledgebase. usually this is only increased.
		// only in cleanup() or manually via check_largest_symbol()
		// you can enforce a check of the complete knowledgebase.
		// NOTE that during a deserialization, this is adjusted as
		//      well.
		{{{
			return largest_symbol;
		}}}
		int check_largest_symbol()
		// checks the complete knowledgebase ( O(n) ),
		// adjusts largest_symbol internally and returns it.
		{{{
			iterator it;
			largest_symbol = 0;

			for(it = this->begin(); it != this->end(); ++it)
				if(it->get_label() > largest_symbol)
					largest_symbol = it->get_label();

			return largest_symbol;
		}}}

		void print(std::ostream &os)
		{{{
			iterator ki;
				std::list<int> w;

			os << "knowledgebase {\n";

			for(ki = this->begin(); ki != this->end(); ki++) {
				os << "\tnode ";
				w = ki->get_word();
				print_word(os, w);
				os << " marked " << ( (ki->is_answered()) ? "!" : ( (ki->is_required()) ? "?" : "%" ) );
				if(ki->is_answered())
					os << " answered " << ki->get_answer();
				os << "\n";
			}

			os << "}\n";
		}}}
		std::string to_string()
		{{{
			std::stringstream str;
			this->print(str);
			return str.str();
		}}}
		std::string visualize()
		{{{
			std::stringstream str;
			iterator it;
			std::string wname;

			str << "digraph knowledgebase {\n"
				"\trankdir=LR;\n"
				"\tsize=8;\n";

			// add all nodes
			for(it = this->begin(); it != this->end(); it++) {
				wname = word2string( it->get_word() );

				str << "\tnode [shape=\"";
				if(it->is_answered())
					str << "ellipse";
				else
					if(it->is_required())
						str << "invhouse";
					else
						str << "box";
				str << "\", style=\"filled\", color=\"";
				if(it->is_answered()) {
					if(typeid(answer) == typeid(bool)) {
						// bool. use value for color
						answer a = it->get_answer();
						bool *b = (bool*)(&a);

						str << ((*b) ? "green" : "red");
					} else {
						// no bool. just indicate we have information
						str << "orange";
					}
				} else {
					if(it->is_required())
						str << "cyan";
					else
						str << "gray95";
				}
				str << "\", label=\"" << wname << " [" << it->timestamp << "]";
				if(it->is_answered())
					str << " := '" << it->get_answer() << "'\"] \"k" << wname << "\";\n";
				else
					str << "\"] \"k" << wname << "\";\n";
			}

			// and add all connections
			for(it = this->begin(); it != this->end(); it++) {
				typename std::vector<node *>::iterator ci;
				std::string toname;

				wname = word2string( it->get_word() );
				for(ci = it->children.begin(); ci != it->children.end(); ci++) {
					if(*ci) {
						toname = word2string( (*ci)->get_word() );
						str << "\t\"k" << wname << "\" -> \"k" << toname << "\" [label=\"" << (*ci)->label << "\"];\n";
					}
				}
			}

			str << "}\n";
			return str.str();
		}}}
		std::string visualize(equivalence_relation & eq)
		// FIXME: use operator<< fot it->get_answer().
		{{{
			std::string ret;

			char buf[128];
			iterator it;

			std::list<int> word;
			std::string wname;

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
				typename std::vector<node *>::iterator ci;
				std::string toname;

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
			std::set<node*> done;
			typename equivalence_relation::iterator ni;
			for(ni = eq.begin(); ni != eq.end(); ni++) {
				if(done.find(ni->second) == done.end()) {
					std::set<node*> eq_class;
					typename std::set<node*>::iterator si;
					std::list<node*> sorted_eq_class;

					// sort equivalence class, then draw a line through the class
					eq_class = eq.get_equivalence_class(ni->second);
					int count;
					for(si = eq_class.begin(), count = 0; si != eq_class.end(); si++, count++) {
						done.insert(*si);
						// sorted insert into sorted_eq_class: [FIXME: efficiency is non-existant]
						typename std::list<node*>::iterator li;
						for(li = sorted_eq_class.begin(); li != sorted_eq_class.end(); li++)
							if( ! (*li)->is_graded_lex_smaller(*si) )
								break;
						sorted_eq_class.insert(li, *si);
					}

					if(count >= 2) {
						typename std::list<node*>::iterator li1, li2;
						li1 = sorted_eq_class.begin();
						li2 = li1;
						li2++;

						while(li2 != sorted_eq_class.end()) {
							std::list<int> w1, w2;
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


		std::basic_string<int32_t> serialize() const
		{{{
			std::basic_string<int32_t> ret;

			ret += 0; // sizeof, will be filled in later

			ret += ::serialize(resolved_queries);
			root->serialize_subtree(ret);

			ret[0] = htonl(ret.size() - 1);

			return ret;
		}}}
		bool deserialize(serial_stretch & ser)
		{{{
			// NOTE: the following members implicitly get values during the
			// deserialization of the tree:
			//	nodecount
			//	answercount
			//	largest_symbol
			//	timestamp

			int size;

			clear();

			if(!::deserialize(size, ser)) goto failed;
			if(!::deserialize(resolved_queries, ser)) goto failed;

			if(ser.empty()) goto failed;
			if(((int)ntohl(*ser)) != -1) goto failed; // label of root-node must be -1.
			if(!root->deserialize_subtree(ser)) goto failed;

			return true;

		failed:
			clear();
			return false;
		}}}
		bool deserialize_query_acceptances(serial_stretch & ser)
		// answer all queries from a single, serialized chunk.
		// the expected format is:
		//	int length (i.e. the number of upcoming integers, excluding this one.
		//			AND ALSO THE EXACT NUMBER OF QUERIES IN THIS KBASE)
		//	int answer[] (that is for each query, exactly one answer);
		// NOTE: the expected order of the answers can be obtained in three ways:
		// 1) you use create_query_tree(). the queries are ordered in expected way
		// by their timestamps. 2) using get_queries(). 3) by iterating over all
		// queries (via qbegin() and qend())
		{{{
			using libalf::deserialize;
			int size;
			iterator ki;

			if(!::deserialize(size, ser)) goto failed;

			while(size && !required.empty()) {
				answer a;
				if(!deserialize(a, ser)) goto failed;
				required.front()->set_answer(a);
				--size;
			}

			if(size == 0 && required.empty())
				return true;

		failed:
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
				std::list<int> qw;
				qw = qi->get_word();
				query_tree->add_query(qw, 0);
				query_tree->timestamp++;
			}

			return query_tree;
		}}}
		std::list<std::list<int> > get_queries()
		// get list of all queries (in correct order so that deserialize_query_acceptances() may be used)
		{{{
			iterator ki;
			std::list<std::list<int> > ret;

			for(ki = this->qbegin(); ki != this->qend(); ++ki)
				ret.push_back(ki->get_word());

			return ret;
		}}}
		bool merge_knowledgebase(knowledgebase & other_tree)
		// only merges answered information, no queries!
		// returns false if knowledge of the trees is inconsistent.
		// in this case, no changes are applied.
		//
		// all new knowledge will have the same timestamp!
		{{{
			iterator ki;
			int static_timestamp = timestamp; // we want one timestamp for
			// the whole merge, so we have to keep it static!

			// first, check if both knowledgebases are consistent
			if(this->get_rootptr()->recursive_different(other_tree.get_rootptr(), -1))
				return false;

			for(ki = other_tree.begin(); ki != other_tree.end(); ++ki)
				if(ki->is_answered()) {
					std::list<int> w;
					w = ki->get_word();
					if(!this->add_knowledge(w, ki->get_answer()))
						/* this should never happen as we
						 * already checked for consistency */
						{};
					timestamp = static_timestamp;
				}
			// increment timestamp so we mark one complete merge with a single timestamp
			timestamp++;
			return true;
		}}}

		bool add_knowledge(std::list<int> & word, answer acceptance)
		// will return false if knowledge for this word was
		// already set and is != acceptance. in this case, the
		// holder is in an inconsistent state and the
		// knowledgebase will not change itself.
		{{{
			return root->find_or_create_descendant(word.begin(), word.end())->set_answer(acceptance);
		}}}
		int add_query(std::list<int> & word, int prefix_count = 0)
		// returns the number of new required nodes (excluding
		// those already known.
		{{{
			node * current;
			std::list<int>::iterator wi;
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
		bool resolve_query(std::list<int> & word, answer & acceptance)
		// returns true if known
		// will also try to apply the filter, if set.
		// if the filter knows the answer, the answer will not
		// be stored in the knowledgebase but returned to the user.
		{{{
			const node * current;

			current = root->find_descendant(word.begin(), word.end());

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
		bool resolve_or_add_query(std::list<int> & word, answer & acceptance)
		// returns true if known. otherwise marks knowledge as
		// to-be-acquired and returns false.
		// will also try to apply the filter, if set.
		// if the filter knows the answer, it will be stored into the
		// knowledgebase.
		{{{
			node * current;

			current = root->find_or_create_descendant(word.begin(), word.end());

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
		node* get_nodeptr(std::list<int> & word)
		// get node* for a specific word
		{{{
			return root->find_or_create_descendant(word.begin(), word.end());
		}}}
		bool node_exists(const std::list<int> & word) const
		{{{
			return (root->find_descendant(word.begin(), word.end()) != NULL);
		}}};
		bool knowledge_exists(const std::list<int> & word) const
		{{{
			node * n;
			n = root->find_descendant(word.begin(), word.end());
			if(n)
				return n->is_answered();
			else
				return false;
		}}};
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

		bool equivalence_relation2automaton(equivalence_relation &eq, bool ignoring_states_accept, finite_automaton &automaton)
		// construct a modulo automaton given the tree-structure
		// in this knowledgebase and the equivalence relation.
		// will return false if the equivalence relation is
		// inconsistent, i.e. it merges states that are answered
		// with different knowledge.
		{{{
			// get a set of all representatives
			std::set<node*> representatives;
			typename std::set<node*>::iterator si;
			std::map<node*, int> mapping;
			int i;

			// map representatives to state-IDs
			representatives = eq.representatives_ptr();
			for(i=0, si = representatives.begin(); si != representatives.end(); i++, si++)
				mapping[*si] = i;

			automaton.clear();
			automaton.input_alphabet_size = -1;
			automaton.is_deterministic = false;
			automaton.state_count = representatives.size();;
			automaton.initial_states.insert(mapping[eq.representative_ptr(get_rootptr())]);

			// get final states and transitions
			std::vector<extended_bool> acceptances;
			extended_bool b;
			b.value = extended_bool::EBOOL_UNKNOWN;
			for(i = 0; i < automaton.state_count; i++)
				acceptances.push_back(b);

			typename equivalence_relation::iterator eqi;
			node *n = NULL;
			for(eqi = eq.begin(); eqi != eq.end(); eqi++) {
				if(eqi->first != n) {
					n = eqi->first;
					int childcount = n->max_child_count();

					if(automaton.input_alphabet_size < childcount)
						automaton.input_alphabet_size = childcount;

					int src = mapping[eq.representative_ptr(n)];
					for(i = 0; i < childcount; i++) {
						// add transition if exists
						node *c;
						c = n->find_child(i);
						if(c != NULL) {
							automaton.transitions[src][i].insert(mapping[eq.representative_ptr(c)]);
						}
					}
					if(n->is_answered()) {
						if(n->get_answer() == true) {
							if(acceptances[src].value == extended_bool::EBOOL_FALSE) {
								std::list<int> word;
								word = n->get_word();
//								printf("inconsistency in equivalence relation: %s\n", word2string(word).c_str());
								return false;
							}
							acceptances[src].value = extended_bool::EBOOL_TRUE;
						} else {
							if(n->get_answer() == false) {
								if(acceptances[src].value == extended_bool::EBOOL_TRUE) {
									std::list<int> word;
									word = n->get_word();
//									printf("inconsistency in equivalence relation: %s\n", word2string(word).c_str());
									return false;
								}
								acceptances[src].value = extended_bool::EBOOL_FALSE;
							} else {
								// well what can we do :)
							}
						}
					}
				}
			}

			for(i = 0; i < automaton.state_count; i++) {
				if(acceptances[i].value == extended_bool::EBOOL_TRUE)
					automaton.output_mapping[i] = true;
				else
					automaton.output_mapping[i] = (acceptances[i].value == extended_bool::EBOOL_UNKNOWN && ignoring_states_accept);
			}

			return true;
		}}}

}; // end of knowledgebase


template<typename answer>
std::basic_string<int32_t> serialize(typename knowledgebase<answer>::node * n)
// this will ONLY serialize the word, not any information about membership!
{{{
	return ::serialize(n->get_word());
}}}

template<typename answer>
bool deserialize(typename knowledgebase<answer>::node * & into, knowledgebase<answer> & base, serial_stretch & serial)
{{{
	std::list<int> w;

	into = NULL;
	if(!::deserialize(w, serial)) return false;
	into = base.get_rootptr()->find_or_create_descendant(w.begin(), w.end());
	return true;
}}}


// classes to iterate over a full subtree, in graded lexicographic order:
// PURE FORWARD ITERATOR

// iterate in graded lex. order:
// a < b  iff  |a| < |b| or |a|==|b| && (a <[LEX] b)
template <class answer>
class kIterator_lex_graded {
	private:
		std::queue<typename knowledgebase<answer>::node*> pending;
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
			typename std::vector<typename knowledgebase<answer>::node*>::iterator ci;
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
		kIterator_lex_graded operator++(int __attribute__ ((__unused__)) foo)
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

