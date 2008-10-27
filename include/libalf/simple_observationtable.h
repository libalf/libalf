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

#ifndef __libalf_simple_observationtable_h__
# define __libalf_simple_observationtable_h__

#include <list>
#include <vector>
#include <algorithm>
#include <functional>

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/observationtable.h>
#include <libalf/automata_amore.h>

#include "amore/dfa.h"

namespace libalf {

using namespace std;

namespace simple_ot {

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

};

// simple observation table for angluin learning algorithm
template <class answer>
class simple_observationtable : observationtable<answer> {

	private:
		typedef vector< list<int> > columnlist;
		columnlist column_names;

		typedef list< simple_ot::simple_row<answer> > rowlist;
		rowlist upper_table;
		rowlist lower_table;

		teacher<answer> & teach;
		logger & log;
		int alphabet_size;

	public:
		simple_observationtable()
		{{{
			teach = NULL;
			log = NULL;
			alphabet_size = 0;
		}}}

		simple_observationtable(teacher<answer> & teach, logger & log, int alphabet_size)
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

			complete();
		}}}

		virtual void set_teacher(teacher<answer> & teach)
		{{{
			this->teach = teach;
		}}}

		virtual teacher<answer> & get_teacher()
		{{{
			return teach;
		}}}

		virtual void set_logger(logger & l)
		{{{
			log = l;
		}}}

		virtual logger & get_logger()
		{{{
			return log;
		}}}

		virtual void undo()
		{{{
			  if(log)
				  log(LOGGER_ERROR, "simple_observationtable::undo() is not implemented.\naborting.\n");

			  // FIXME: throw exception
		}}}

		virtual void redo()
		{{{
			  if(log)
				  log(LOGGER_ERROR, "simple_observationtable::redo() is not implemented.\naborting.\n");

			  // FIXME: throw exception
		}}}

		virtual void savetofile(char* filename)
		{
			
		}

		virtual void loadfromfile(char* filename)
		{
			
		}

		virtual list< list<int> > &get_columns()
		{{{
			return column_names;
		}}}

		// searches column first, then row
		virtual pair<bool, answer> check_entry(list<int> word)
		{{{
			columnlist::iterator ci;
			typename rowlist::iterator uti, lti;

			pair<bool, answer> ret;

			// find possible suffixes in table columns
			for(ci = column_names.begin(); ci < column_names.end(); ci++) {
				if(is_suffix_of(*ci, word)) {
					// find possible prefix as column index
					// then return truth value
					list<int> prefix;
					list<int>::iterator prefix_end;
					prefix_end = word.begin();
					prefix_end += word.size() - ci->size();
					prefix.assign(word.begin(), prefix_end);

					uti = search_upper_table(prefix);
					if(uti != upper_table.end()) {
						ret.first = true;
						ret.second = uti->acceptance[ci];
					}
					lti = search_lower_table(prefix);
					if(lti != lower_table.end()) {
						ret.first = true;
						ret.second = lti->acceptance[ci];
					}
				}
			}

			// word is not in table
			ret.first = false;
			ret.second = false;
			return ret;
		}}}

		virtual automata * derive_hypothesis()
		{{{
			complete();
			return derive_automata();
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
				uti->acceptance.pop_front();
				uti->acceptance.push_front(a);
			}
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

		virtual typename rowlist::iterator search_lower_table(list<int> &word)
		{{{
			typename rowlist::iterator lti;

			for(lti = lower_table.begin(); lti != lower_table.end(); lti++)
				if(word == lti->index)
					return lti;
			return lower_table.end();
		}}}

		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true)
		{{{
			simple_ot::simple_row<answer> row;

			if(check_uniq)
				if(search_upper_table(word) != -1 || search_lower_table != -1)
					return;

			// add the word to the upper table
			row.index = word;
			upper_table.push_back(row);

			// add all suffixes of word to lower table
			for( int i = 0; i < alphabet_size; i++ ) {
				if(check_uniq)
					if(search_upper_table(word) != -1) // can't be in lower table, as its prefix would be in upper then
						continue;
				word.push_back(i);
				row.index = word;
				lower_table.push_back(row);
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
					ci += column_names.size() - uti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						w = uti->index + *ci;
						uti->push_back(teach.membership_query(*w));
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
					ci += column_names.size() - lti->acceptance.size();
					for(/* -- */; ci != column_names.end(); ci++) {
						list<int> *w;
						w = lti->index + *ci;
						lti->push_back(teach.membership_query(*w));
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
		// returns true if table was closed before,
		//         false if table was changed (and thus needs to be filled)
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
					lti.erase();
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
		// FIXME: only checking lower_table for +1 successors?
		{{{
			bool urow_ok[upper_table.size()];
			bool lrow_ok[lower_table.size()];
			typename rowlist::iterator uti_1, uti_2, lti_1, lti_2;
			int i,j;
			int k,l;

			for(i = 0; i < upper_table.size(); i++)
				urow_ok[i] = false;
			for(k = 0; k < upper_table.size(); k++)
				lrow_ok[k] = false;

			for(i = 0, uti_1 = upper_table.begin(); uti_1 != (upper_table.end() - 1); i++, uti_1++) {
				if(urow_ok[i])
					continue;
				urow_ok[i] = true;

				for(uti_2 = uti_1+1, j=i+1; uti_2 != upper_table.end(); uti_2++, j++) {
					if(urow_ok[j])
						continue;
					if(uti_1 == uti_2) {
						// [uti_1].acceptance == [uti_2].acceptance
						// -> test if all equal suffixes result in equal acceptance as well
						for(lti_1 = lower_table.begin(), k=0; lti_1 != lower_table.end(); lti_1++, k++) {
							// FIXME: check if this optimization is ok
							if(lrow_ok[k])
								continue;
							if(   is_prefix_of(uti_1->index, lti_1->index)
							   && (uti_1->index.size() + 1 == lti_1->index.size())) {
								// find matching prefix-row for upper_table[uti_2].index
								// check for equal acceptance of both prefix rows
								list<int>::iterator suffix;
								suffix = lti_1->index.begin();
								suffix += uti_1->index.size();
		// XXX

								list<int> w;
								w.assign(upper_table[uti_2].index.begin(), upper_table[uti_2].index.end());
								for(/* -- */; suffix < lower_table[lti_1].index.end(); suffix++)
									w.push_back(*suffix);
								int lti_2 = search_lower_table(w);

								if(lti_2 >= 0)
									if(lower_table[lti_1] != lower_table[lti_2])
										return false;
		// XXX
								lrow_ok[k] = true;
								lrow_ok[l] = true;
							}
						}
					}
				}
			}
			return true;
		}}}

		// make table consistent: perform operations to do that.
		// returns true if table was consistent before,
		//         false if table was changed (and thus needs to be filled)
		virtual bool make_consistent()
		{
			
		}

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

		virtual automata * derive_automata()
		// FIXME: possibly refactor this into the automata interface, so this
		//    can be used with any automata implementation
		//    (will have extra overhead, but so what?)
		{
			// derive deterministic finite automata from this table
			dfa dfa_p;
			dfa_p = newdfa();

			// list of states of automata: each different acceptances-row
			// in the upper table represents one DFA state
			
			// q0 is row(\epsilon)
			
			// the final, accepting states are the rows with
			// acceptance in column \epsilon
			
			// the transformation function is:
			// \delta: (row, char) -> row :: (row(s), a) -> row(sa)
			

			automata_amore *a;
			a = new automata_amore();
			a->set_dfa(dfa_p);
			return a;
		}


};

}; // end of namespace libalf

#endif

