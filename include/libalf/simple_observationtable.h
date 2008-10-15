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

namespace libalf {

namespace simple_ot {
template <class answer>
class simple_row {
	public:
		list<int> index;
		vector<answer> acceptance;

		bool equal_acceptance(simple_row<answer> &other)
		{{{
			return  (acceptance == other.acceptance);
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
			// add epsilon as column
			list<int> word;
			word.push_back(ALPHABET_EPSILON);
			column_names.push_back(word);
			// add epsilon as row in upper table
			simple_ot::simple_row<answer> row;
			row.index = word;
			upper_table.push_back(row);

			// add each char as row in lower table
			for( int i = ALPHABET_FIRST; i <= alphabet_size; i++ ) {
				simple_ot::simple_row<answer> row;

				word.clear();
				word.push_back(i);
				row.index = word;

				lower_table.push_back(row);
			}

			set_teacher(teach);
			this->alphabet_size = alphabet_size;
			set_logger(log);
			complete();
		}}}

		virtual teacher<answer> & get_teacher()
		{{{
			return teach;
		}}}

		virtual void set_teacher(teacher<answer> & teach)
		{{{
			this->teach = teach;
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
		// FIXME
		{{{
		}}}

		virtual void loadfromfile(char* filename)
		// FIXME
		{{{
		}}}

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

		// sample implementation only
		virtual bool is_closed()
		{{{
			for(int lti = 0; lti < lower_table.size(); lti++) {
				simple_ot::simple_row<answer> & r = lower_table[lti];
				bool match_found = false;

				for(int uti = 0; uti < upper_table.size(); uti++) {
					if(r.equal_acceptance(upper_table[uti])) {
						match_found = true;
						break;
					}
				}
				if(!match_found)
					return false;
			}
			return true;
		}}}

		// sample implementation only
		virtual bool is_consistent()
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
					if(upper_table[uti_1].equal_acceptance(upper_table[uti_2])) {
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
									if(!lower_table[lti_1].equal_acceptance(lower_table[lti_2]))
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

		virtual void complete()
		// FIXME
		{{{
			// first complete all missing fields by querying the teacher for membership

			// upper table
			for(int uti = 0; uti < upper_table.size(); uti++) {
				if(upper_table[uti].acceptance.size() < column_names.size()) {
					// fill in missing acceptance information
					int ci = column_names.size() - upper_table[uti].acceptance.size();
					for(/* -- */; ci < column_names.size(); ci++) {
						list<int> *w;
						w = upper_table[uti].index + column_names[ci];
						upper_table[uti].acceptance[ci] = teach.membership_query(*w);
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
						lower_table[lti].acceptance[ci] = teach.membership_query(*w);
						delete w;
					}
				}
			}

			// second check, if table is closed and consistent.
			// if not, change it in that way and complete again.

			// FIXME

		}}}

};

}; // end of namespace libalf

#endif

