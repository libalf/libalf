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

#ifndef __libalf_oracle_automaton_h__
# define __libalf_oracle_automaton_h__

#include <list>

#include <libalf/automata.h>
#include <libalf/oracle.h>

namespace libalf {

using namespace std;

// oracle automaton
//
// simple oracle that uses an automaton as backend for all its tests
//
class oracle_automaton : public oracle {
	private:
		finite_language_automaton *backend_automaton;
	public:
		oracle_automaton();

		oracle_automaton(finite_language_automaton &a);

		virtual ~oracle_automaton();

		virtual void set_automaton(finite_language_automaton &a);

		virtual finite_language_automaton *get_automaton();

		// returns <true, (void)> if automaton is ok,
		// returns <false, list of counter-examples> if automaton is not ok
		virtual pair<bool, list< list<int> > > equality_query(finite_language_automaton & hypothesis);
};

}; // end namespace libalf

#endif

