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
		vector< list<int> > column_names;
		vector< simple_ot::simple_row<answer> > upper_table;
		vector< simple_ot::simple_row<answer> > lower_table;

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
			int ci, uti, lti;
			pair<bool, answer> ret;

			// find possible suffixes in table columns
			for(ci = 0; ci < column_names.size(); ci++) {
				if(is_suffix_of(column_names[ci], word)) {
					// find possible prefix as column index
					// then return truth value
					list<int> prefix;
					list<int>::iterator prefix_end;
					prefix_end = word.begin();
					prefix_end += word.size() - column_names[ci].size();
					prefix.assign(word.begin(), prefix_end);

					uti = search_upper_table(prefix);
					if(uti >= 0) {
						ret.first = true;
						ret.second = upper_table[uti].acceptance[ci];
					}
					lti = search_lower_table(prefix);
					if(lti >= 0) {
						ret.first = true;
						ret.second = upper_table[lti].acceptance[ci];
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
			// add word and all prefixes to upper table
			while(prefix.size() > 0) {
				add_word_to_upper_table(prefix);
				prefix.pop_back();
			}

			int uti = search_upper_table(word);
			if(upper_table[uti].acceptance.size() == 0) {
				upper_table[uti].acceptance.push_back(a);
			} else {
				// XXX SHOULD NEVER HAPPEN
				upper_table[uti].acceptance.pop_front();
				upper_table[uti].acceptance.push_front(a);
			}
		}}}


	protected:
		virtual int search_upper_table(list<int> &word)
		{{{
			for(int uit = 0; uit < upper_table.size(); uit++) {
				if(word == upper_table[uit].index)
					return uit;
			}
			return -1;
		}}}

		virtual int search_lower_table(list<int> &word)
		{{{
			for(int lit = 0; lit < lower_table.size(); lit++)
				if(word == lower_table[lit].index)
					return lit;
			return -1;
		}}}

		virtual void add_word_to_upper_table(list<int> word, bool check_uniq = true)
		{{{
			simple_ot::simple_row<answer> row;

			if(check_uniq)
				if(search_upper_table(word) != -1 || search_lower_table != -1)
					return;

			// add the word the the upper table
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
			// upper table
			for(int uti = 0; uti < upper_table.size(); uti++) {
				if(upper_table[uti].acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					int ci = column_names.size() - upper_table[uti].acceptance.size();
					for(/* -- */; ci < column_names.size(); ci++) {
						list<int> *w;
						w = upper_table[uti].index + column_names[ci];
						upper_table[uti].push_back(teach.membership_query(*w));
						delete w;
					}
				}
			}
			// lower table
			for(int lti = 0; lti < lower_table.size(); lti++) {
				if(lower_table[lti].acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					int ci = column_names.size() - lower_table[lti].acceptance.size();
					for(/* -- */; ci < column_names.size(); ci++) {
						list<int> *w;
						w = lower_table[lti].index + column_names[ci];
						lower_table[lti].push_back(teach.membership_query(*w));
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
			for(int lti = 0; lti < lower_table.size(); lti++) {
				simple_ot::simple_row<answer> & r = lower_table[lti];
				bool match_found = false;

				for(int uti = 0; uti < upper_table.size(); uti++) {
					if(r == upper_table[uti]) {
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

			for(int lti = 0; lti < lower_table.size(); lti++) {
				simple_ot::simple_row<answer> & r = lower_table[lti];
				bool match_found = false;

				for(int uti = 0; uti < upper_table.size(); uti++) {
					if(r == upper_table[uti]) {
						match_found = true;
						break;
					}
				}
				if(!match_found) {
					// create entry in upper table
					add_word_to_upper_table(r.index, false);
					int i = search_upper_table(r.index);
					// copy acceptance status for that row
					swap(upper_table[i].acceptance, r.acceptance);
					// delete lower entry
					typename vector< simple_ot::simple_row<answer> >::iterator it;
					it = lower_table.begin();
					it += lti;
					it.erase();

					changed = true;
				}
			}

			return changed;
		}}}

		// sample implementation only:
		//  for all _equal_ rows in upper table: all +1 successors over all
		//  members of alphabet have to have equal rows
		virtual bool is_consistent()
		// FIXME: refactor. this is way too big.
		{{{
			bool urow_ok[upper_table.size()];
			bool lrow_ok[lower_table.size()];
			int uti_1;
			for(uti_1 = 0; uti_1 < upper_table.size(); uti_1++)
				urow_ok[uti_1] = false;

			for(uti_1 = 0; uti_1 < upper_table.size() - 1; uti_1++) {
				if(urow_ok[uti_1])
					continue;
				urow_ok[uti_1] = true;

				for(int uti_2 = uti_1+1; uti_2 < upper_table.size(); uti_2++) {
					if(urow_ok[uti_2])
						continue;
					if(upper_table[uti_1] == upper_table[uti_2]) {
						// [uti_1].acceptance == [uti_2].acceptance
						// -> test if all equal suffixes result in equal acceptance as well
						for(int lti_1 = 0; lti_1 < lower_table.size(); lti_1++) {
							// FIXME: check if this optimization is ok
							if(lrow_ok[lti_1])
								continue;
							if(   is_prefix_of(upper_table[uti_1].index, lower_table[lti_1].index)
							   && (upper_table[uti_1].index.size() + 1 == lower_table[lti_1].index.size())) {
								// find matching prefix-row for upper_table[uti_2].index
								// check for equal acceptance of both prefix rows
								list<int>::iterator suffix;
								suffix = lower_table[lti_1].index.begin();
								suffix += upper_table[uti_1].index.size();

								list<int> w;
								w.assign(upper_table[uti_2].index.begin(), upper_table[uti_2].index.end());
								for(/* -- */; suffix < lower_table[lti_1].index.end(); suffix++)
									w.push_back(*suffix);
								int lti_2 = search_lower_table(w);

								if(lti_2 >= 0)
									if(lower_table[lti_1] != lower_table[lti_2])
										return false;
								lrow_ok[lti_1] = true;
								lrow_ok[lti_2] = true;
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

