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

enum extended_bool {
	EB_TRUE,
	EB_FALSE,
	EB_UNKNOWN
};

template <class answer>
class teacher {

	public:
		virtual answer membership_query(list< int >) = 0;

		virtual void membership_query(BDD< int >) = 0;

};

}; // end namespace libalf

