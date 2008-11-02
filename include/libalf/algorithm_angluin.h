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

#ifndef __libalf_algorithm_angluin_h__
# define __libalf_algorithm_angluin_h__

#include <list>
#include <vector>
#include <algorithm>
#include <functional>
#include <ostream>

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>
#include <libalf/automata.h>

namespace libalf {

using namespace std;

	namespace algorithm_angluin {

		template <class answer>
		class simple_row {
			public:
				list<int> index;
				vector<answer> acceptance;

				bool operator==(simple_row<answer> &other)
				{{{
					return (acceptance == other.acceptance);
				}}}

				bool operator!=(simple_row<answer> &other)
				{{{
					return (acceptance != other.acceptance);
				}}}

				bool operator>(simple_row<answer> &other)
				{{{
					typename vector<answer>::iterator ai;
					typename vector<answer>::iterator oai;

					ai = acceptance.begin();
					oai = other.acceptance.begin();

					for(/* -- */; ai < acceptance.end() && oai < other.acceptance.end(); ai++, oai++) {
						if(*ai > *oai)
							return true;
					}

					return false;
				}}}
		};

		template <class answer>
		class automaton_state {
			public:
				int id;
				typedef list< algorithm_angluin::simple_row<answer> > rowlist;
				typename rowlist::iterator tableentry;
		};

	};

// simple observation table for angluin learning algorithm
template <class answer>
class simple_observationtable : learning_algorithm<answer> {

	private:
		typedef vector< list<int> > columnlist;
		columnlist column_names;

		typedef list< algorithm_angluin::simple_row<answer> > rowlist;
		rowlist upper_table;
		rowlist lower_table;

		teacher<answer> * teach;
		logger * log;
		int alphabet_size;

	public:
		simple_observationtable()
		{{{
			teach = NULL;
			log = NULL;
			alphabet_size = 0;
		}}}

		simple_observationtable(teacher<answer> * teach, logger * log, int alphabet_size)
		{{{
			this->alphabet_size = alphabet_size;

			set_teacher(teach);
			set_logger(log);

			list<int> word; // empty word!

			// add epsilon as column
			column_names.push_back(word);

			// add epsilon to upper table
			// and all suffixes to lower table
			add_word_to_upper_table(word, false);
		}}}

		virtual void set_teacher(teacher<answer> * teach)
		{{{
			this->teach = teach;
		}}}

		virtual teacher<answer> * get_teacher()
		{{{
			return teach;
		}}}

		virtual void set_logger(logger * l)
		{{{
			log = l;
		}}}

		virtual logger * get_logger()
		{{{
			return log;
		}}}

		virtual void undo()
		{{{
			  if(log)
				  (*log)(LOGGER_ERROR, "simple_observationtable::undo() is not implemented.\naborting.\n");

			  // FIXME: throw exception
		}}}

		virtual void redo()
		{{{
			  if(log)
				  (*log)(LOGGER_ERROR, "simple_observationtable::redo() is not implemented.\naborting.\n");

			  // FIXME: throw exception
		}}}

		virtual void savetofile(char* filename)
		{
			
		}

		virtual void loadfromfile(char* filename)
		{
			
		}

		virtual void print(ostream &os)
		{{{
			typename columnlist::iterator ci;
			typename rowlist::iterator ti;
			typename vector<answer>::iterator vi;

			os << "simple_observationtable {\n";
			os << "\tcolumns:";

			for(ci = column_names.begin(); ci != column_names.end(); ci++) {
				os << " ";
				print_word(os, *ci);
			}
			os << " ;\n";

			os << "\tupper table:\n";
			for(ti = upper_table.begin(); ti != upper_table.end(); ti++) {
				os << "\t\t";
				print_word(os, ti->index);
				os << ": ";
				for(vi = ti->acceptance.begin(); vi != ti->acceptance.end(); vi++) {
					if(*vi == true)
						os << "+ ";
					else
						if(*vi == false)
							os << "- ";
						else
							os << "? ";
				}
				os << ";\n";
			}

			os << "\tlower_table:\n";
			for(ti = lower_table.begin(); ti != lower_table.end(); ti++) {
				os << "\t\t";
				print_word(os, ti->index);
				os << ": ";
				for(vi = ti->acceptance.begin(); vi != ti->acceptance.end(); vi++) {
					if(*vi == true)
						os << "+ ";
					else
						if(*vi == false)
							os << "- ";
						else
							os << "? ";
				}
				os << ";\n";
			}

			os << "}\n";
		}}}

		// searches column first, then row
		virtual pair<bool, answer> check_entry(list<int> word)
		{{{
			columnlist::iterator ci;
			unsigned int col_index;
			typename rowlist::iterator uti, lti;

			pair<bool, answer> ret;

			// find possible suffixes in table columns
			for(ci = column_names.begin(), col_index = 0; ci < column_names.end(); ci++, col_index++) {
				if(is_suffix_of(*ci, word)) {
					// find possible prefix as column index
					// then return truth value
					list<int> prefix;
					list<int>::iterator prefix_end;
					prefix_end = word.begin();
					for(int n = word.size() - ci->size(); n > 0; n--)
						prefix_end++;
					//prefix_end += word.size() - ci->size();
					prefix.assign(word.begin(), prefix_end);

					uti = search_upper_table(prefix);
					if(uti != upper_table.end()) {
						ret.first = true;
						ret.second = uti->acceptance[col_index];
						return ret;
					}
					lti = search_lower_table(prefix);
					if(lti != lower_table.end()) {
						ret.first = true;
						ret.second = lti->acceptance[col_index];
						return ret;
					}
				}
			}

			// word is not in table
			ret.first = false;
			ret.second = false;
			return ret;
		}}}

		virtual bool derive_hypothesis(finite_language_automaton * automaton)
		{{{
			complete();
			// XXX: check that this is a DFA?
			return derive_automaton(automaton);
		}}}

		virtual void add_counterexample(list<int> word, answer a)
		{{{
			list<int> prefix = word;
			int ps;

			// add word and all prefixes to upper table
			for(ps = prefix.size(); ps > 0; ps--) {
				add_word_to_upper_table(prefix);
				prefix.pop_back();
			}

			typename rowlist::iterator uti = search_upper_table(word);
			if(uti->acceptance.size() == 0) {
				uti->acceptance.push_back(a);
			} else {
				// XXX SHOULD NEVER HAPPEN
				uti->acceptance[0] = a;
			}
		}}}

		virtual void add_counterexample(list<int> word)
		{{{
			list<int> prefix = word;

			// add word and all prefixes to upper table
			while(!prefix.empty()) {
				add_word_to_upper_table(prefix);
				prefix.pop_back();
			}
		}}}

		virtual list< list<int> > *get_columns()
		{{{
			list< list<int> > *l = new list< list<int> >();
			typename columnlist::iterator ci;

			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				l->push_back(*ci);

			return l;
		}}}

	protected:
		virtual typename rowlist::iterator search_upper_table(list<int> &word)
		{{{
			typename rowlist::iterator uti;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(word == uti->index)
					return uti;
			}
			return upper_table.end();
		}}}

		virtual typename rowlist::iterator search_upper_table(list<int> &word, int &index)
		{{{
			typename rowlist::iterator uti;
			index = 0;

			for(uti = upper_table.begin(); uti != upper_table.end(); uti++, index++) {
				if(word == uti->index)
					return uti;
			}
			index = -1;
			return upper_table.end();
		}}}

		virtual typename rowlist::iterator search_lower_table(list<int> &word)
		{{{
			typename rowlist::iterator lti;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++)
				if(word == lti->index)
					return lti;
			return lower_table.end();
		}}}

		virtual typename rowlist::iterator search_lower_table(list<int> &word, int &index)
		{{{
			typename rowlist::iterator lti;
			index = 0;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++, index++)
				if(word == lti->index)
					return lti;
			index = -1;
			return lower_table.end();
		}}}

		virtual typename rowlist::iterator search_tables(list<int> &word)
		{{{
			typename rowlist::iterator it;

			it = search_upper_table(word);
			if(it != upper_table.end())
				return it;

			return search_lower_table(word);
		}}}

		virtual typename rowlist::iterator search_tables(list<int> &word, bool &upper_table, int&index)
		{{{
			typename rowlist::iterator it;

			it = search_upper_table(word, index);
			if(index != -1) {
				return it;
			}

			return search_lower_table(word, index);
		}}}

		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true)
		{{{
			algorithm_angluin::simple_row<answer> row;
			bool done = false;

			if(check_uniq) {
				if(search_upper_table(word) != upper_table.end()) {
					return;
				}
				typename rowlist::iterator ti;
				ti = search_lower_table(word);
				if(ti != lower_table.end()) {
					done = true;
					upper_table.push_back(*ti);
					lower_table.erase(ti);
				}
			}

			// add the word to the upper table
			if(!done) {
				row.index = word;
				upper_table.push_back(row);
			}

			// add all suffixes of word to lower table
			for( int i = 0; i < alphabet_size; i++ ) {
				word.push_back(i);
				done = false;
				if(check_uniq)
					if(search_upper_table(word) != upper_table.end()) {
						// can't be in lower table, as its prefix would be in upper then
						done = true;
					}
				if(!done) {
					row.index = word;
					lower_table.push_back(row);
				}
				word.pop_back();
			}
		}}}

		virtual void fill_missing_columns()
		{{{
			typename rowlist::iterator uti, lti;
			// upper table
			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				if(uti->acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					columnlist::iterator ci;
					ci = column_names.begin();
					ci += uti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						w = concat(uti->index, *ci);
						uti->acceptance.push_back(teach->membership_query(*w));
						delete w;
					}
				}
			}
			// lower table
			for(lti = lower_table.begin(); lti != lower_table.end(); lti++) {
				if(lti->acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					columnlist::iterator ci;
					ci = column_names.begin();
					ci += lti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						w = concat(lti->index, *ci);
						lti->acceptance.push_back(teach->membership_query(*w));
						delete w;
					}
				}
			}
		}}}

		// sample implementation only:
		//  all possible answer-rows in
		//  lower table already exist in upper table
		//  (for angluin)
		virtual bool is_closed()
		{{{
			typename rowlist::iterator uti, lti;
			for(lti = lower_table.begin(); lti != lower_table.end(); lti++) {
				bool match_found = false;

				for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
					if(*lti == *uti) {
						match_found = true;
						break;
					}
				}
				if(!match_found)
					return false;
			}
			return true;
		}}}

		// close table: perform operations to close it.
		// returns false if table was closed before,
		//         true if table was changed (and thus needs to be filled)
		virtual bool close()
		{{{
			bool changed = false;
			typename rowlist::iterator uti, lti, tmplti;

			for(lti = lower_table.begin(); lti != lower_table.end(); /* -- */) {
				bool match_found = false;

				for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
					if(*lti == *uti) {
						match_found = true;
						break;
					}
				}
				if(!match_found) {
					// create entry in upper table
					add_word_to_upper_table(lti->index, false);
					typename rowlist::iterator last = upper_table.end();
					last--; // FIXME: does this work?
					// copy acceptance status for that row
					swap(last->acceptance, lti->acceptance);
					// go to next and delete old lower entry
					tmplti = lti;
					tmplti++;
					//lti.erase();
					lower_table.erase(lti);
					lti = tmplti;

					changed = true;
				} else {
					// go to next
					lti++;
				}
			}

			return changed;
		}}}

		// sample implementation only:
		//  for all _equal_ rows in upper table: all +1 successors over all
		//  members of alphabet have to have equal rows
		virtual bool is_consistent()
		{{{
			bool urow_ok[upper_table.size()];
			typename rowlist::iterator uti_1, uti_2, ut_last_row;
			unsigned int i,j;

			for(i = 0; i < upper_table.size(); i++)
				urow_ok[i] = false;

			ut_last_row = upper_table.end();
			ut_last_row--;

			for(i = 0, uti_1 = upper_table.begin(); uti_1 != ut_last_row; i++, uti_1++) {
				if(urow_ok[i])
					continue;
				urow_ok[i] = true;
				uti_2 = uti_1;
				uti_2++;
				for(j=i+1; uti_2 != upper_table.end(); uti_2++, j++) {
					if(urow_ok[j])
						continue;
					if(*uti_1 == *uti_2) {
						// uti_1->acceptance == uti_2->acceptance
						// -> test if all equal suffixes result in equal acceptance as well
						list<int> word1 = uti_1->index;
						list<int> word2 = uti_2->index;
						typename rowlist::iterator w1succ, w2succ;
						for(int sigma = 0; sigma < alphabet_size; sigma++) {
							word1.push_back(sigma);
							w1succ = search_tables(word1);

							word2.push_back(sigma);
							w2succ = search_tables(word2);

							if(*w1succ != *w2succ)
								return false;

							word1.pop_back();
							word2.pop_back();
						}
					}
				}
			}
			return true;
		}}}

		virtual void add_column(list<int>word)
		{{{
			typename columnlist::iterator ci;

			for(ci = column_names.begin(); ci != column_names.end(); ci++)
				if(*ci == word)
					return;

			column_names.push_back(word);
		}}}

		// make table consistent: perform operations to do that.
		// returns false if table was consistent before,
		//         true if table was changed (and thus needs to be filled)
		virtual bool make_consistent()
		{{{
			bool changed = false;

			bool urow_ok[upper_table.size()];
			typename rowlist::iterator uti_1, uti_2, ut_last_row;
			unsigned int i,j;

			for(i = 0; i < upper_table.size(); i++)
				urow_ok[i] = false;

			ut_last_row = upper_table.end();
			ut_last_row--;

			for(i = 0, uti_1 = upper_table.begin(); uti_1 != ut_last_row; i++, uti_1++) {
				if(urow_ok[i])
					continue;
				urow_ok[i] = true;
				uti_2 = uti_1;
				uti_2++;
				for(j=i+1; uti_2 != upper_table.end(); uti_2++, j++) {
					if(urow_ok[j])
						continue;
					if(*uti_1 == *uti_2) {
						// uti_1->acceptance == uti_2->acceptance
						// -> test if all equal suffixes result in equal acceptance as well
						list<int> word1 = uti_1->index;
						list<int> word2 = uti_2->index;
						typename rowlist::iterator w1_succ, w2_succ;
						for(int sigma = 0; sigma < alphabet_size; sigma++) {
							word1.push_back(sigma);
							w1_succ = search_tables(word1);

							word2.push_back(sigma);
							w2_succ = search_tables(word2);

							if(*w1_succ != *w2_succ) {
								if(w1_succ->acceptance.size() == w2_succ->acceptance.size()) {
									// add suffixes resulting in different states to column_names
									changed = true;

									typename columnlist::iterator ci;
									typename vector<answer>::iterator w1_acc_it, w2_acc_it;

									ci = column_names.begin();
									w1_acc_it = w1_succ->acceptance.begin();
									w2_acc_it = w2_succ->acceptance.begin();

									while(w1_acc_it != w1_succ->acceptance.end()) {
										if(*w1_acc_it != *w2_acc_it) {
											list<int> newsuffix;

											// generate and add suffix
											newsuffix = *ci;
											newsuffix.push_front(sigma);
											add_column(newsuffix);
										}
										w1_acc_it++;
										w2_acc_it++;
										ci++;
									}
								}
							}

							word1.pop_back();
							word2.pop_back();
						}
					}
				}
			}

			return changed;
		}}}

		virtual void complete()
		{{{
			// first complete all missing fields by querying the teacher for membership
			fill_missing_columns();

			if(make_consistent()) {
				complete();
				return;
			}

			// second check, if table is closed and consistent.
			// if not, change it in that way and complete recursively.
			if(close()) {
				complete();
				return;
			}
		}}}

		virtual bool derive_automaton(finite_language_automaton * automaton)
		{{{
			// derive deterministic finite automaton from this table
			typename rowlist::iterator uti, ti;

			algorithm_angluin::automaton_state<answer> state;
			list<algorithm_angluin::automaton_state<answer> > states;
			state.id = 0;
			typename list<algorithm_angluin::automaton_state<answer> >::iterator state_it, state_it2;

			list<int> initial;

			list<int> final;

			list<transition> transitions;

			// list of states of automaton: each different acceptance-row
			// in the upper table represents one DFA state
			for(uti = upper_table.begin(); uti != upper_table.end(); uti++) {
				bool known = false;
				for(state_it = states.begin(); state_it != states.end(); state_it++) {
					if(*uti == *(state_it->tableentry)) {
						// state is already known. skip.
						known = true;
						break;
					}
				}
				if(known)
					continue;

				state.tableentry = uti;

				states.push_back(state);

				state.id++;
			}

			// q0 is row(epsilon)
			// as epsilon is the first row in uti, it will have id 0.
			initial.push_back( 0 );

			for(state_it = states.begin(); state_it != states.end(); state_it++) {
				// the final, accepting states are the rows with
				// acceptance in the epsilon-column
				if(state_it->tableentry->acceptance.front() == true)
					final.push_back(state_it->id);

				// the transformation function is:
				// \delta: (row, char) -> row : (row(s), a) -> row(sa)
				list<int> index;
				typename rowlist::iterator ri;
				index = state_it->tableentry->index;
				for(int i = 0; i < alphabet_size; i++) {
					// find successor in table
					index.push_back(i);
					ti = search_tables(index);

					// find matching state for successor
					for(state_it2 = states.begin(); state_it2 != states.end(); state_it2++) {
						if(*ti == *(state_it2->tableentry)) {
							transition tr;

							tr.source = state_it->id;
							tr.sigma = i;
							tr.destination = state_it2->id;
							transitions.push_back(tr);
							break;
						}
					}

					index.pop_back();
				}
			}

			return automaton->construct(alphabet_size, states.size(),
						initial, final, transitions);
		}}}

};

}; // end of namespace libalf

#endif

