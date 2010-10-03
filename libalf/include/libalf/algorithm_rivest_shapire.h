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
 * (c) 2010 David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_algorithm_rivest_shapire_h__
# define __libalf_algorithm_rivest_shapire_h__

#include <libalf/algorithm_angluin.h>

namespace libalf {

using namespace std;

/*
 * Rivest Shapire Extension of Angluins L* algorithm.
 * It tries to find an optimal split in a given counterexample
 */

template <class answer>
class rivest_shapire_table : public angluin_simple_table<answer> {
	protected: // data
		bool counterexample_mode;
		vector<int> counterexample;
		list<int> counterexample_l;
		answer cex_answer;
		bool cex_answer_set;
		int cex_front, cex_back, cex_latest_bad;

		libalf::finite_automaton latest_cj;
		typedef list< algorithm_angluin::simple_row<answer, vector<answer> > > table_t;
		list<algorithm_angluin::automaton_state<table_t> > latest_cj_statemapping;


	protected:
	public: // methods
		rivest_shapire_table()
		{{{
			this->set_alphabet_size(0);
			this->set_knowledge_source(NULL);
			this->set_logger(NULL);
			counterexample_mode = false;
			cex_front = -1;
			cex_back = -1;
			cex_latest_bad = -1;
			latest_cj.valid = false;
		}}}
		rivest_shapire_table(knowledgebase<answer> *base, logger *log, int alphabet_size)
		{{{
			this->set_alphabet_size(alphabet_size);
			this->set_logger(log);
			this->set_knowledge_source(base);
			counterexample_mode = false;
			cex_front = -1;
			cex_back = -1;
		}}}
		virtual bool complete()
		{
			if(!this->initialized) {
				this->initialize_table();
				counterexample_mode = false;
				latest_cj.valid = false;
			}

			if(!counterexample_mode) {
				// normal operation, almost like angluin L*

				if(   this->fill_missing_columns(this->upper_table)
				   && this->fill_missing_columns(this->lower_table) ) {

					if(!this->close())
						return complete();
//				// OBSOLETE for RV, as the table is always consistent:
//					if(!make_consistent())
//						return complete();

					return true;
				} else{
					return false;
				}
			} else {
				// the basic idea behind the RV algorithm is to analyse a counterexample
				// to find an optimal suffix that discriminated between two stated that
				// were merged in the hypothesis. this suffix is than added as a new column.
				// we do this using a binary search through all possible suffixes, in
				// the bound of cex_front and cex_back. cex_latest_bad marks the latest
				// position that is found to be descriminating between the hypothesis
				// and teacher.

				answer a;
				if(!cex_answer_set)
					cex_answer_set = resolve_or_add_query(counterexample_l, cex_answer);
				if(!cex_answer_set)
					return false;

				if(cex_back < cex_front)
				{{{ // end is reached, cex is invalid
					(*this->my_logger)(LOGGER_ERROR, "rivest_shapire_table: you gave an invalid counterexample. aborting counterexample mode.\n");
					counterexample_mode = false;
					cex_front = -1;
					cex_back = -1;
					cex_latest_bad = -1;
					cex_answer_set = false;

					return false;
				}}}

				// binary search for the state that needs to be split
				int current = (cex_front + cex_back) / 2;

				// check if this suffix discriminates
				list<int> new_word;

				// and add suffix
				list<int>::iterator li;
				int i;
				for(li = counterexample_l.begin(), i = 0; i < current; ++i, ++li)
					new_word.push_back(*li);


				


				if(!resolve_or_add_query(new_word, a))
					return false;

				// compare hypothesis and teacher and advance accordingly.
				// i.e. either continue binary search or add new suffix
				{{{
					if(cex_front != cex_back) {
						// advance binary search until we find the first bad state
						if(a != cex_answer) {
							cex_latest_bad = current;
							cex_back = current-1;
						} else {
							cex_front = current+1;
						}

						return complete();
					} else {
						if(a != cex_answer)
							cex_latest_bad = current;

						// add new suffix to table that discriminated the two states
						while(cex_latest_bad > 0) {
							counterexample_l.pop_front();
							cex_latest_bad--;
						}

						if(!this->add_column(counterexample_l))
							(*this->my_logger)(LOGGER_ERROR, "rivest_shapire_table: invalid counterexample.\n");

						counterexample_mode = false;
						cex_front = -1;
						cex_back = -1;
						cex_latest_bad = -1;
						cex_answer_set = false;

						return complete();
					}
				}}}
			}
		}
		virtual bool add_counterexample(list<int> word)
		{{{
			list<int>::iterator wi;
			if(this->my_knowledge == NULL) {
				(*this->my_logger)(LOGGER_ERROR, "rivest_shapire_table: add_counterexample() without knowledgebase!\n");
				return false;
			}

			if(!latest_cj.valid) {
				(*this->my_logger)(LOGGER_ERROR, "rivest_shapire_table: add_counterexample() without having a conjecture stored! did you ever advance until you had a conjecture?\n");
				return false;
			}

			// check for increase in alphabet size
			int new_asize = this->get_alphabet_size();
			bool asize_changed = false;
			for(wi = word.begin(); wi != word.end(); wi++) {
				if(*wi >= new_asize) {
					new_asize = *wi+1;
					asize_changed = true;
				}
			}
			if(asize_changed) {
				(*this->my_logger)(LOGGER_ALGORITHM, "rivest_shapire_table: add_counterexample(): increase of alphabet_size from %d to %d.\nNOTE: it is possible that the next hypothesis does not increase in state-count.\n", this->get_alphabet_size(), new_asize);
				this->increase_alphabet_size(new_asize);
			}

			this->counterexample.resize(word.size());
			counterexample_l.clear();
			typename list<int>::iterator li;
			int i;
			for(i = 0, li = word.begin(); li != word.end(); ++li, ++i) {
				counterexample[i] = *li;
				counterexample_l.push_back(*li);
			};

			counterexample_mode = true;
			cex_answer_set = false;
			cex_front = 0;
			cex_back = i-1;
			cex_latest_bad = -1;

			if(cex_back < cex_front) {
				(*this->my_logger)(LOGGER_ERROR, "rivest_shapire_table: you gave epsilon as a counterexample. why didn't you set the right answer in the beginning? aborting counterexample mode. if epsilon is really wrong, please restart with a fresh instance.\n");
				counterexample_mode = false;
				cex_front = -1;
				cex_back = -1;
			}
		}}}
		virtual conjecture * derive_conjecture()
		{{{
			conjecture * cj;
			cj = this->derive_conjecture(latest_cj_statemapping);
			if(cj)
				latest_cj = *cj;
			else
				latest_cj.valid = false;
			return cj;
		}}}
};


}; // end of namespace libalf

#endif // __libalf_algorithm_rivest_shapire_h__

