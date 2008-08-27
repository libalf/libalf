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

#include <libalf/alphabet.h>
#include <libalf/automata.h>

namespace libalf {

template <class alphabet>
class oracle {

	// returns a list of counter-examples
	virtual list< list< alphabet > >
	equality_query(automata<alphabet>&);

};

}; // end namespace libalf

