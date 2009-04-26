/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * DFAenumerator: enumerate over all accessible n-state automata with specific alphabet size
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <vector>
#include <set>

#ifndef __LanguageGenerator_DFAenumerator_h__
# define __LanguageGenerator_DFAenumerator_h__

#include <LanguageGenerator/automaton_constructor.h>

namespace LanguageGenerator {

using namespace std;

// for alphabet_size >= 2, DFAenumerator will construct all possible DFAs
// with a given alphabet size and exact state count (i.e. all states reachable)
//
// use derive() to obtain the current DFA,
// use next() to move to the next automaton.

class DFAenumerator {
	private:
		bool completed;

		int alphabet_size;
		int state_count;
		set<int> initial;
		vector<bool> final;
		vector<transition> transitions;

	public:
		DFAenumerator(int state_count, int alphabet_size);

		// returns true if next automaton is valid
		// (all states are reachable)
		// false, if next automaton may be skipped or
		// if all automata have been generated (check generation_completed() ).
		bool next(bool exact_state_count);

		bool derive(LanguageGenerator::automaton_constructor & automaton);

		bool generation_completed()
		{ return completed; };
};

}; // end of namespace LanguageGenerator

#endif

