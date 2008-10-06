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

#include <list>

namespace libalf {

// possible <answer> class: e.g. extended bool or just bool
// (see teacher.h)


// basic interface for different implementations (e.g. one table and one tree)
template <class answer>
class observationtable {

		// horizontal members
		// upper table: U
		// lower talbe: U+{sigma}
	public:
		observationtable() {
		};

		observationtable(teacher<answer> *t, int alphabet_size) {
			// should construct and query:
			// upper table: epsilon
			// lower table: {sigma}
			// columns: epsilon
		};

		virtual void set_teacher(teacher<answer> *) = 0;
		virtual teacher<answer> * get_teacher() = 0;

		virtual void undo() = 0;
		virtual void redo() = 0;

		virtual void savetofile() = 0;
		virtual void loadfromfile() = 0;

		virtual list< list<int> > get_columns() = 0;

		virtual void add_counterexample(list< answer >, answer) = 0;
		// automatically prefix_close, postfix_close

		virtual pair<bool, answer> check_entry(list<int>) = 0;
			// if status unknown, return (false, ?)
			// otherwise return (true, <answer>)

		virtual void suffix_close(teacher) = 0;

	protected:
		virtual void prefix_close(teacher /* ? */) = 0;

		virtual void postfix_close(teacher) = 0;

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

