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
		class node;

		class candidate {
			public:
				typename knowledgebase<answer>::node * word;
				node * position; // position in tree
				bool is_state; // state candidate or just a transition?
			public:
				candidate(typename knowledgebase<answer>::node *word, node *position, bool is_state)
				{{{
					this->word = word;
					this->position = position;
					this->is_state = is_state;
				}}}
				void make_state()
				{{{
					is_state = true;
				}}}
		};

		class node {
			public:
				list<int> suffix; // splitting suffix of this node
				bool is_leaf;
				list<candidate *> candidates; // candidates (pending, if this is not a leaf)
				node *accepting, *rejecting;
			public:
				node()
				{{{
					is_leaf = false;
					accepting = NULL;
					rejecting = NULL;
				}}};
				~node()
				{{{
					if(!is_leaf) {
						delete accepting;
						delete rejecting;
					}
				}}}
				int get_memory_usage()
				{{{
					int ret = sizeof(this);
					if(!is_leaf) {
						ret += accepting->get_memory_usage();
						ret += rejecting->get_memory_usage();
					}
					return ret;
				}}}
		};

	protected: // data
		node * tree; // suffix-splitting tree

		list<candidate*> pending; // all pending candidates (that have not yet reached a leaf in the tree)

		map<typename knowledgebase<answer>::node*, candidate*> candidates; // ALL candidates
		set<candidate*> final; // NOTE: also contains transitions going to final states. this is maintained in sift_pending().

	public:
		angluin_tree()
		{{{
			tree = NULL;
			this->set_alphabet_size(0);
			this->set_knowledge_source(NULL);
			this->set_logger(NULL);
		}}}
		angluin_tree(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			tree = NULL;
			this->set_alphabet_size(alphabet_size);
			this->set_knowledge_source(base);
			this->set_logger(log);
		}}}
		virtual ~angluin_tree()
		{{{
			typename map<typename knowledgebase<answer>::node*, candidate*>::iterator ci;

			if(tree)
				delete tree;
			for(ci = candidates.begin(); ci != candidates.end(); ++ci)
				delete ci->second;
		}}}

		virtual void new_candidate(typename knowledgebase<answer>::node* word, bool is_state)
		{{{
			candidate * c = new candidate(word, tree, is_state);
			tree->candidates.push_back(c);
			candidates[word] = c;
		}}}

		virtual void increase_alphabet_size(int new_asize)
		{{{
			if(new_asize <= this->get_alphabet_size())
				return;

			typename map<typename knowledgebase<answer>::node*, candidate*>::iterator ci;

			// add new transitions for all states.
			for(ci = candidates.begin(); ci != candidates.end(); ++ci)
				for(int new_suffix = this->get_alphabet_size(); new_suffix < new_asize; ++new_suffix)
					new_candidate(ci->second->word->find_or_create_child(new_suffix), false);

			this->set_alphabet_size(new_asize);
		}}}
		virtual void get_memory_statistics(statistics & stats)
		// table_size.words is an approximation!
		{
			
		}
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
			if(pending.size() > 0)
				return false;
			// check consistency and closedness
			
		}
		virtual void add_counterexample(list<int>)
		{
			
		}

	protected:
		bool sift_pending()
		{{{
			bool new_required = false;
			if(pending.size() == 0)
				return true;

			typename list<candidate*>::iterator pi;

			for(pi = pending.begin(); pi != pending.end(); ++pi) {
				typename knowledgebase<answer>::node *w;
				bool sift_done = (*pi)->position->is_leaf;

				while(!sift_done) {
					// get word from prefix and new suffix
					w = (*pi)->word->find_or_create_child((*pi)->position->suffix.begin(), (*pi)->position->suffix.end());
					if(w->is_answered()) {
						// move candidate to subtree
						typename list<candidate*>::iterator d;
						for(d = (*pi)->position->candidates.begin(); d != (*pi)->position->candidates.end(); ++d)
							if(*d == *pi)
								break;
						(*pi)->position->candidates.erase( d );

						// mark final-status now.
						if((*pi)->position == tree)
							if(w->get_answer() == true)
								final.insert(*pi);

						if(w->get_answer() == true)
							(*pi)->position = (*pi)->position->accepting;
						else
							(*pi)->position = (*pi)->position->rejecting;

						(*pi)->position->candidates.push_back(*pi);
						if((*pi)->position->is_leaf) {
							sift_done = true;
							pi = pending.erase(pi);
						};
					} else {
						// unable to sift until we're at a leaf.
						// a teacher needs to answer the queries in the knowledgebase.
						w->mark_required();
						new_required = true;
						sift_done = true;
					}
				}
			}

			return new_required;
		}}}

		bool is_closed()
		{
			// for each non-state candidate check if there is a state-candidate at the same leaf.
			typename map<typename knowledgebase<answer>::node*, candidate*>::iterator ci;

			// create a set with all transition-candidates
			set<candidate*> transitions;
			for(ci = candidates.begin(); ci != candidates.end(); ++ci)
				if(!(*ci)->is_state)
					transitions.insert(*ci);

			// for all state-candidates, remove transitions going into them.
			for(ci = candidates.begin(); ci != candidates.end(); ++ci)
				if((*ci)->is_state) {
					typename list<candidate*>::iterator pi;
					for(pi = (*ci)->position->candidates.begin(); pi != (*ci)->position->candidates.end(); ++pi)
						transitions.erase(*pi);
				}

			// if there is a remainder, the table is not closed.
			return transitions.empty();
		}
		bool close()
		{
			// for each non-state candidate check if there is a state-candidate at the same leaf.
			// if not, make it a state-candidate.
			
		}

		bool is_consistent()
		{
			// check if for all candidates at same leaf their transitions go to the same leaves.
			
		}
		bool make_consistent()
		{
			// check if for all candidates at same leaf their transitions go to the same leaves.
			// if not, find splitting suffix and add it at the current leaf.
			
		}

		virtual bool complete()
		{{{
			if(sift_pending()) {
				if(!close())
					return complete();
				if(!make_consistent())
					return complete();

				return true;
			} else {
				return false;
			}
		}}}

		virtual bool derive_automaton(bool & is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
		{
			
		}

};


}; // end of namespace libalf

#endif

