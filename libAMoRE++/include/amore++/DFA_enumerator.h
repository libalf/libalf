/* $Id$
 * vim: fdm=marker
 *
 * amore++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <vector>
#include <set>

#include <amore++/deterministic_finite_automaton.h>

#ifndef __amore_dfa_enumerator_h__
# define __amore_dfa_enumerator_h__

namespace amore {

using namespace std;

// for alphabet_size >= 2, DFA_enumerator will construct all possible DFAs
// with a given alphabet size and exact state count (i.e. all states reachable)
//
// use derive() to obtain the current DFA,
// use next() to move to the next automaton.

class DFA_enumerator {
	private:
		bool completed;

		int alphabet_size;
		int state_count;
		set<int> initial;
		vector<bool> final;
		vector<transition> transitions;

	public:
		DFA_enumerator(int state_count, int alphabet_size);

		// returns true if next automaton is valid
		// (all states are reachable)
		// false, if next automaton may be skipped or
		// if all automata have been generated (check generation_completed() ).
		bool next(bool exact_state_count);

		deterministic_finite_automaton * derive();

		bool generation_completed()
		{ return completed; };
};

}; // end of namespace amore

#endif

