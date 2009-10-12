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
 *    and 2009 by Daniel Neider,    Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <neider@automata.rwth-aachen.de>
 *
 */



// PLEASE NOTE: KVtree is currently not working. we found a serious bug and will
// have to rewrite it almost completely :-(



#ifndef __libalf_algorithm_kvtree_h__
# define __libalf_algorithm_kvtree_h__

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <ostream>
#include <sstream>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

// KV_tree - algorithm as described in section 8.3 (Exact Learning of Finite Automata) of
//	* M.J. Kearns, U.V. Vazirani: "An Introduction To Computational Learning Theory",
//	  1994, MIT Press, ISBN 978-0-262-11193-5

// NOTES on algorithm: active online algorithm, thus using membership- and equivalence queries.
// needs exactly n equivalence queries for a model of size n.
// on the other hand, the amount of required membership queries is minimal.
// intermediate hypothesis may differ vastly from original model.

template <class answer>
class KVtree: public learning_algorithm<answer> {
	/* in learning_algorithm: *my_logger, *my_knowledge, *norm, alphabet_size */
	protected: // types
		class node;

		class transition {
			public:
				node * current_position;
				knowledgebase::node * word;
				leaf * src;
				int label;
				node * dst;
			public:
				candidate()
				{ current_position = NULL; word = NULL; src = NULL; dst = NULL; label = -2; }
				~candidate()
				{ }
		};


		class node {
			public:
				node * parent;
				int depth; // (root node has depth 0)

				knowledgebase::node * word;
			public:
				node()
				{ word = NULL; parent = NULL; depth = -1; }
				virtual ~node()
				{ };
				node * greatest_common_parent(node* other)
				{{{
					node * me = this;

					// align on same depth
					while(me->depth != other->depth && me != NULL && other != NULL) {
						if(me->depth < other->depth)
							other = other->parent;
						else
							me = me->parent;
					}
					// find common parent
					while(me != other) {
						if(me->parent == NULL || other->parent == NULL)
							return NULL;
						me = me->parent;
						other = other->parent;
					}
					return me;
				}}}
				virtual int get_memory_usage() = 0;
		};
		class internal : public node {
			public:
				node * accepting;
				node * rejecting;
			public:
				virtual ~internal()
				{ delete accepting; delete rejecting; }
				virtual int get_memory_usage()
				{{{
					return sizeof(class internal)
						+ accepting->get_memory_usage()
						+ rejecting->get_memory_usage()
						- (parent == NULL ? sizeof(class internal) : 0);
				}}}
		};
		class leaf : public node {
			public:
				int state_id;
				bool is_initial;
				bool is_final;
				vector<transition> outgoing;
				list<transition*> incoming;
			public:
				leaf()
				{ state_id = -1; is_initial = false; is_final = false; }
				virtual ~leaf()
				{ }
				virtual int get_memory_usage()
				{{{
					return sizeof(class leaf)
						+ sizeof(transition) * incoming.size();
				}}}
				leaf* simulate_run(list<int>::iterator word_begin, list<int>::iterator word_end)
				{{{
					if(word_begin == word_end)
						return this;

					list<int>::iterator label;

					label = word_begin;
					word_begin++;
					return outgoing[label].simulate_run(word_begin, word_end);
				}}}
		};

	protected: // data

		// 0: no initialization at all; 1: tree with only epsilon node; 2: normal tree
		int initialized;

		// tree, pending candidates and all leaf-nodes in tree
		node tree;
		list<candidate*> pending;
		list<leaf*> leaves;
		int current_state_count;

		bool oh_valid; // does an old hypothesis exist?


	public:
		KVtree(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_knowledge_source(base);
			this->set_logger(log);

			initialized = false;

			current_state_count = 0;

			oh_valid = false;
		}}}
		virtual ~KVtree()
		{{{
			list<candidate>::iterator pi;

			for(pi = pending.begin(); pi != pending.end(); ++pi)
				delete *pi;
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			if(new_asize <= this->get_alphabet_size())
				return;

			// add new transitions for all states.
			list<leaf*>::iterator li;
			for(li = leaves.begin; li != leaves.end(); ++li)
				for(int new_suffix = this->get_alphabet_size(); new_suffix < new_asize; ++new_suffix)
					new_candidate((*li)->word->find_or_create_child(new_suffix), false, new_suffix);

			this->set_alphabet_size(new_asize);
		}}}
		virtual memory_statistics get_memory_statistics()
		{{{
			memory_statistics ret;
			ret.bytes = sizeof(this) + tree.get_memory_usage() + pending.size() * sizeof(candidate);
			return ret;
		}}}
		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "KVtree: sync-operation not supported but trying to sync!\n");
			return false;
		}}}
		virtual bool supports_sync()
		{{{
			return false;
		}}}
		virtual basic_string<int32_t> serialize()
		{
			basic_string<int32_t> ret;
			// FIXME
			
			return ret;
		}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
			// FIXME
			
			return false;
		}
		virtual void print(ostream &os)
		{{{
			os << tostring();
		}}}
		virtual string tostring()
		{
			string s;
			// FIXME
			
			return s;
		}
		virtual bool conjecture_ready()
		{{{
			return (pending.size() == 0);
		}}}
		virtual void add_counterexample(list<int> word)
		{
			if(this->my_knowledge == NULL) {
				(*this->my_logger)(LOGGER_ERROR, "KVtree: add_counterexample() without knowledgebase!\n");
				return;
			}

			if(!oh_valid) {
				(*this->my_logger)(LOGGER_ERROR, "KVtree: trying to give counterexample but there is no old hypothesis! trying to ignore.\n");
				return;
			}
			if(pending.size() != 0) {
				(*this->my_logger)(LOGGER_ERROR, "KVtree: trying to give counterexample while I am sifting down an old one. trying to ignore.\n");
				return;
			}

			// check for increase in alphabet size
			list<int>::iterator wi;
			bool asize_changed = false;
			int new_asize = this->get_alphabet_size();
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= new_asize) {
					new_asize = *wi+1;
					asize_changed = true;
				}
			}
			if(asize_changed) {
				(*this->my_logger)(LOGGER_ALGORITHM, "KVtree: counterexample: implicit increase of alphabet_size from %d to %d.\n", this->get_alphabet_size(), new_asize);
				increase_alphabet_size(new_asize);
			}

			// prepare sifting of counterexample
			

		}

	protected:
		bool new_candidate(typename knowledgebase<answer>::node* word, bool is_state, bool label = -1)
		// create new candidate or make existing a state candidate.
		// returns false if candidate was already known.
		{
			candidate *c;

			if(is_state) {
				c = new counterexample;
				c->src = 
				c->label = label;
				c->dst = 
			} else {
				c = new transition;
				c->word = word;
			}

			c->current_position = &tree;
			
		}

		bool sift_pending()
		{
			if(initialized == 0) {
				// tree / discriminating node for epsilon already exists
				// but has no children. first add pending epsilon
				list<int> epsilon;
				new_candidate(get_nodeptr(epsilon), true);
				initialized = 1;
				return true;
			}

			if(pending.empty())
				return true;

			
		}

		virtual bool complete()
		{

			return sift_pending();
		}

		// derive an automaton and return it
		virtual conjecture * derive_conjecture()
		{
			simple_automaton *ret = new simple_automaton;

			

			ret->valid = false;

			return ret;
		}


};


}; // end of namespace libalf

#endif

