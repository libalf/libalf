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

#include <libalf/automata.h>

namespace libalf {

class oracle {

	// returns a list of counter-examples
	virtual list< list<int> >
	equality_query(automata<int>&);

};

}; // end namespace libalf
