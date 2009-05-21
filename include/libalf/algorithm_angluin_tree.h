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

#ifndef __libalf_algorithm_angluin_tree_h__
# define __libalf_algorithm_angluin_tree_h__

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

template <class answer>
class angluin_tree : public learning_algorithm<answer> {
	/* in learning_algorithm: *my_logger, *my_knowledge, *norm, alphabet_size */
	protected: // types
		typedef pair<bool, knowledgebase::node*> candidate;
		// <true, X> if it is a state-candidate (would be in upper table),
		// <false, X> if it is a transition-candidate (would be in lower)

		class node {
			public:
				list<int> suffix;
				bool is_leaf;
				list<candidate> pending;
				node *accepting, *rejecting;

				node()
				{ is_leaf = false; accepting = NULL; rejecting = NULL; };

				~node()
				{ if(!is_leaf) { delete accepting; delete rejecting; } };

				void make_internal(list<int> & suffix);
				{{{
					if(is_leaf) {
						this->suffix = suffix;
						accepting = new node;
						rejecting = new node;
					}
				}}}

				int get_memory_usage()
				{{{
					int ret = sizeof(this);
					if(!is_leaf) {
						ret += accepting->get_memory_statistics();
						ret += rejecting->get_memory_statistics();
					}
					return ret;
				}}}

				void count(int & suffixes, int & state_candidates, int & transition_candidates, bool initial_recursion = true)
				{{{
					list<candidate>::iterator ci;

					if(initial_recursion) {
						suffixes = 0;
						state_candidates = 0;
						transition_candidates = 0;
					}

					for(ci = pending.begin(); ci != pending.end(); ++ci)
						if(ci->first)
							state_candidates++;
						else
							transition_candidates++;

					if(is_leaf) {
						suffixes++;
					} else {
						accepting->count(suffixes, state_candidates, transition_candidates, false);
						rejecting->count(suffixes, state_candidates, transition_candidates, false);
					}
				}}}

				int update(knowledgebase * base)
				// update tree: try to move pending downwards.
				// returns number of pending candidates NOT in a leaf
				{{{
					if(is_leaf) {
						return 0;
					} else {
						int internal_candidates = 0;
						list<candidate>::iterator ci;

						ci = pending.begin();
						while(ci != pending.end()) {
							// try to move down by querying knowledgebase
							knowledgebase::node * q;
							q = ci->second->find_or_create_child(suffix.begin(), suffix.end());
							if(q->is_answered()) {
								if(q->get_answer() == true)
									accepting->pending.push_back(*ci);
								else
									rejecting->pending.push_back(*ci);

								ci = pending.erase(ci);
							} else {
								q->mark_required();
								internal_candidates++;

								ci++;
							}
						}

						internal_candidates += accepting->update(base);
						internal_candidates += rejecting->update(base);

						return internal_candidates;
					}
				}}}
		};

	protected: // data
		node * tree;

	public:
		angluin_tree()
		{ tree = NULL; }
		virtual ~angluin_tree()
		{{{
			if(tree)
				delete tree;
		}}}
		virtual void increase_alphabet_size(int new_asize)
		{
		}
		virtual void get_memory_statistics(statistics & stats)
		// table_size.words is an approximation!
		{{{
			stats.table_size.bytes = sizeof(this);
			if(tree) {
				stats.table_size.bytes += tree->get_memory_usage();
				tree->count(stats.table_size.columns, stats.table_size.upper_table, stats.table_size.lower_table);
				stats.table_size.words = (stats.table_size.upper_table + stats.table_size.lower_table) * log(stats.table_size.columns) / log(2);
			}
		}}}
		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "angluin_tree: sync-operation not supported but trying to sync!\n");
			return false;
		}}}
		virtual bool supports_sync()
		{ return false; }
		virtual basic_string<int32_t> serialize()
		{
		}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
		}
		virtual void print(ostream &os)
		{ os << tostring(); }
		virtual string tostring()
		{
		}
		virtual bool conjecture_ready()
		{
		}
		virtual void add_counterexample(list<int>)
		{
		}

	protected:
		virtual bool complete()
		{
			if(tree->update == 0) {
				if(!close())
					return complete();
				if(!make_consistent)
					return complete();

				return true;
			} else {
				return false;
			}
		}
		virtual bool derive_automaton(bool & is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
		{
		}



};


}; // end of namespace libalf

#endif

