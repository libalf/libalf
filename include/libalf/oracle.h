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
#include <libalf/statistics.h>

namespace libalf {

using namespace std;

class oracle {
	protected:
		statistics * stats;
	public:
		oracle()
		{{{
			stats = NULL;
		}}}
		virtual ~oracle() { };
		// returns <true, (void)> if automaton is ok,
		// returns <false, list of counter-examples> if automaton is not ok
		virtual pair<bool, list< list<int> > > equivalence_query(finite_language_automaton &) = 0;

		virtual void set_statistics_counter(statistics * stats)
		{{{
			this->stats = stats;
		}}}
};

}; // end namespace libalf

#endif

