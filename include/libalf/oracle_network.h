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

#ifndef __libalf_oracle_network_h__
# define __libalf_oracle_network_h__

#include <list>

#include <libalf/automata.h>
#include <libalf/oracle.h>

namespace libalf {

using namespace std;

class oracle_network : public oracle, public network_connection {
	public:
		oracle_network();
		virtual ~oracle_network();

		virtual pair<bool, list< list< list<int> > > equivalence_query(finite_language_automaton &a);
};

}; // end namespace libalf

#endif // __libalf_oracle_network_h__

