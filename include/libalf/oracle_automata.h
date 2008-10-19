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

#ifndef __libalf_oracle_automata_h__
# define __libalf_oracle_automata_h__

#include <list>

#include <libalf/automata.h>
#include <libalf/oracle.h>

namespace libalf {

using namespace std;

// oracle automata
//
// simple oracle that uses an automata as backend for all its tests
//
class oracle_automata : public oracle {
	private:
		automata *backend_automata;
	public:
		oracle_automata();

		oracle_automata(automata &a);

		virtual ~oracle_automata();

		virtual void set_automata(automata &a);

		virtual automata *get_automata();

		// returns <true, (void)> if automata is ok,
		// returns <false, list of counter-examples> if automata is not ok
		virtual pair<bool, list< list<int> > > equality_query(automata & hypothesis);
};

}; // end namespace libalf

#endif

