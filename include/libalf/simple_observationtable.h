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
class {
	list<int> index;
	vector<answer> acceptance;
} simple_row;
}

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

		teacher<answer> * get_teacher() {
			return teacher;
		};

		void set_teacher(teacher<answer> * t) {
			teacher = t;

			// FIXME:
			// close table with new teacher, if not closed.
		}

}

}

