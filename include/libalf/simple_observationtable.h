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

#include <libalf/observationtable.h>
#include <libalf/alphabet.h>

namespace libalf {

namespace simple_ot {
template <class answer>
class simple_row {
	public:
		list<int> index;
		vector<answer> acceptance;

		bool equal_acceptance(simple_row<answer> other) {
			for( int col = 0; col < acceptance.size(); col++ )
				if(acceptance[col] != other.acceptance[col])
					return false;
			return false;
		}
};
};

// simple observation table for angluin learning algorithm
template <class answer>
class simple_observationtable :: observationtable<answer> {

	private:
		vector< list<int> > column_names;
		vector< simple_ot::simple_row<answer> > upper_table;
		vector< simple_ot::simple_row<answer> > lower_table;

		teacher<answer> * teacher;

	public:
		simple_observationtable() {
		};

		simple_observationtable(teacher<answer> * t, int alphabet_size) {

			// add epsilon as column
			list<int> word;
			word.push_back(ALPHABET_EPSILON);
			column_names.push_back(word);
			// add epsilon as row in upper table
			simple_row row;
			row.index = word;
			upper_table.push_back(row);

			// add each char as row in lower table
			for( int i = ALPHABET_FIRST; i <= alphabet_size; i++ ) {
				simple_row row;

				word.clear;
				word.push_back(i);
				row.index = word;

				lower_table.push_back(row);
			}

			set_teacher(t);
		};

		virtual teacher<answer> * get_teacher() {
			return teacher;
		};

		virtual void set_teacher(teacher<answer> * t) {
			teacher = t;
			complete();
		}


	protected:
		virtual bool is_closed() {
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
		};

		virtual bool is_consistent() {

		};

		virtual void complete() {
			// first complete all missing fields
			// by querying the teacher for
			// membership

			// FIXME

			// second check, if table is closed and consistent.
			// if not, change it in that way and complete again.

			// FIXME

		}

};

};

