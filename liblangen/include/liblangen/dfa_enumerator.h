/* $Id$
 * vim: fdm=marker
 *
 * liblangen (LANguageGENerator)
 * dfa_enumerator: enumerate over all accessible n-state automata with specific alphabet size
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <list>
#include <map>
#include <vector>
#include <set>

#ifndef __liblangen_dfa_enumerator_h__
# define __liblangen_dfa_enumerator_h__

namespace liblangen {

using namespace std;

// for alphabet_size >= 2, dfa_enumerator will construct all possible DFAs
// with a given alphabet size and exact state count (i.e. all states reachable)
//
// use derive() to obtain the current DFA,
// use next() to move to the next automaton.

class dfa_enumerator {
	private:
		typedef struct transition_t {
			int source;
			int label;
			int destination;
		} transition;
	private:
		bool completed;

		int alphabet_size;
		int state_count;
		set<int> initial;
		vector<bool> final;
		vector<transition> transitions;

	public:
		dfa_enumerator(int state_count, int alphabet_size);

		// returns true if next automaton is valid
		// (all states are reachable)
		// false, if next automaton may be skipped or
		// if all automata have been generated (check generation_completed() ).
		bool next(bool exact_state_count);

		bool derive(bool &t_is_dfa, int &t_alphabet_size, int &t_state_count, std::set<int> &t_initial, std::set<int> &t_final, multimap<pair<int,int>, int> &t_transitions);

		bool generation_completed()
		{ return completed; };
};

}; // end of namespace liblangen

#endif
