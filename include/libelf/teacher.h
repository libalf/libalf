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

enum extended_bool {
	EB_TRUE,
	EB_FALSE,
	EB_UNKNOWN
};

template <class alphabet, class answer>
class teacher {

	public:
		virtual answer membership_query(list< alphabet >) = 0;

		virtual void membership_query(BDD< alphabet >) = 0;

};

}; // end namespace libalf

