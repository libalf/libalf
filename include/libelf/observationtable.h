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

#include <alphabet.h>

namespace libalf {

// possible <answer> class: e.g. extended bool or just bool
// (see teacher.h)


// basic interface for different implementations (e.g. one table and one tree)
template <class alphabet, class answer>
class observationtable {

	public:
		virtual bool is_closed() = 0;

		virtual bool is_consistent() = 0;

		virtual void add_entry(list< alphabet>, answer) = 0;
		virtual pair<bool, answer> check_entry(list< alphabet>) = 0;
			// if status unknown, return (false, ?)
			// otherwise return (true, <answer>)

		virtual void close() = 0;

};

}; // end namespace libalf

