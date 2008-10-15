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

#ifndef __libalf_oracle_h__
# define __libalf_oracle_h__

#include <list>

#include <libalf/automata.h>

namespace libalf {

using namespace std;

class oracle {
	public:
		// returns a list of counter-examples
		virtual list< list<int> > equality_query(automata<int>&) = 0;
};

}; // end namespace libalf

#endif

