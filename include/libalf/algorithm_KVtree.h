/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __libalf_algorithm_kvtree_h__
# define __libalf_algorithm_kvtree_h__

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <ostream>
#include <sstream>

#include <arpa/inet.h>

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

// KV_tree - algorithm as described in section 8.3 (Exact Learning of Finite Automata) of
// "M.J. Kearns, U.V. Vazirani - An Introduction To Computational Learning Theory",
// 1994, MIT Press, ISBN 978-0-262-11193-5

// NOTES on algorithm: active online algorithm, thus using membership- and equivalence queries.
// needs exactly n equivalence queries for a model of size n.
// on the other hand, the amount of required membership queries is minimal.
// intermediate hypothesis may differ vastly from original model.

template <class answer>
class KVtree: public learning_algorithm<answer> {
	/* in learning_algorithm: *my_logger, *my_knowledge, *norm, alphabet_size */
	protected: // types
		class node;

		class candidate {
			public:
				node * current_position;
			public:
				candidate()
				{ current_position = NULL };
		};
		class transition_dst : public candidate {
			public:
				int source;
				int label;
			public:
				transition_dst()
				{ source = -1; label = -1; };
		};
		class counterexample : public candidate {
			public:
				list<int> word;
		};


		class node {
			public:
				knowledgebase::node * word;
			public:
				virtual int get_memory_usage() = 0;
				virtual ~node() { };
		};
		class internal : public node {
			public:
				node * accepting;
				node * rejecting;
			public:
				virtual ~internal()
				{ delete accepting; delete rejecting; }
				virtual int get_memory_usage()
				{ return sizeof(class internal) + accepting->get_memory_usage() + rejecting->get_memory_usage(); }
		};
		class leaf : public node {
			public:
				int state_id;
				bool is_final;
				list<transition_dst*> incoming;
			public:
				leaf()
				{ state_id = -1; }
				virtual ~leaf()
				{ while(!incoming.empty()) { delete incoming.front(); incoming.pop_front(); } }
				virtual int get_memory_usage()
				{ return sizeof(class leaf) + sizeof(transition_dst) * incoming.size(); }
		};

	protected: // data

		bool initialized;

		// tree, pending candidates and all leaf-nodes in tree
		node * tree;
		list<candidate*> pending;
		list<leaf*> leaves;
		int current_state_count;

		// last hypothesis:
		bool lh_valid;
		int lh_alphabet_size;
		int lh_state_count;
		int lh_initial;
		set<int> lh_final;
		multimap<pair<int, int>, int> lh_transitions;


	public:
		KVtree(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_knowledge_source(base);
			this->set_logger(log);

			initialized = false;

			tree = NULL;
			current_state_count = 0;

			lh_valid = false;
		}}}
		virtual ~KVtree()
		{{{
			list<candidate>::iterator pi;

			if(tree)
				delete tree;
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
					new_candidate((*li)->word->find_or_create_child(new_suffix), false);

			this->set_alphabet_size(new_asize);
		}}}
		virtual void get_memory_statistics(statistics & stats)
		// table_size.words is an approximation!
		{{{
			stats.table_size.bytes = sizeof(this) + tree->get_memory_usage() + pending.size() * sizeof(candidate);
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
			if(!lh_valid) {
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
		bool new_candidate(typename knowledgebase<answer>::node* word, bool is_state)
		// create new candidate or make existing a state candidate.
		// returns false if candidate was already known.
		{
			
		}

		bool sift_pending()
		{
			
		}

		virtual bool complete()
		{ return sift_pending(); }

		virtual bool derive_automaton(bool & is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
		{
			
		}

};


}; // end of namespace libalf

#endif

