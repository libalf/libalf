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
		virtual ~oracle() { };
		// returns <true, (void)> if automata is ok,
		// returns <false, list of counter-examples> if automata is not ok
		virtual pair<bool, list< list<int> > > equality_query(automata&) = 0;
};

}; // end namespace libalf

#endif

