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

#ifndef __libalf_observationtable_h__
# define __libalf_observationtable_h__

#include <list>
#include <utility>

#include <libalf/teacher.h>

namespace libalf {

using namespace std;

// basic interface for different implementations (e.g. one table and one tree)
template <class answer>
class observationtable {

	public:
		observationtable() {
		};

		virtual void set_teacher(teacher<answer> *) = 0;
		virtual teacher<answer> * get_teacher() = 0;

		virtual void undo() = 0;
		virtual void redo() = 0;

		virtual void savetofile(char* filename) = 0;
		virtual void loadfromfile(char* filename) = 0;

		// return a reference to the column-names
		// never change this!
		virtual list< list<int> > &get_columns() = 0;

		virtual void add_counterexample(list< answer >, answer) = 0;
		// automatically prefix_close, postfix_close

		virtual pair<bool, answer> check_entry(list<int>) = 0;
			// if status unknown, return (false, ?)
			// otherwise return (true, <answer>)

	protected:
		virtual bool is_closed() = 0;
		// all possible answer-rows in
		// lower table already exist in upper table
		// (for angluin)
		//
		// every answer-row from lower table can be simulated/combined
		// by rows from the upper table
		// (for RFSA [NFA])

		virtual bool is_consistent() = 0;
		// for all _equal_ rows in upper table: all +1 successors over all
		// members of alphabet have to be equal
		// (for angluin)
		//
		// 1) if row 1 <= row 2 implies that row 1 + {alpha} <= row 2 + {alpha}
		// 2) if row 1 = SUM(row n...m) implies that row 1 + {alpha} = SUM(row n {alpha} ... row m {alpha})
		// (for RFSA [NFA])

};

}; // end namespace libalf

#endif

