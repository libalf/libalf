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

#ifndef __libalf_automaton_h__
# define __libalf_automaton_h__

#include <set>
#include <map>
#include <string>

namespace libalf {

using namespace std;

// create nice dotfile from automaton
string automaton2dotfile(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);


// parse human readable description of automaton
bool read_automaton(string input, bool is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);

// create human readable description of automaton
string write_automaton(bool is_dfa, int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);


// parse machine-readable description of automaton
// (this format is compatible to the libAMoRE++ format.
// NOTE: it does not store if the automaton is deterministic or not!)
bool deserialize_automaton(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit,
			int & alphabet_size, int & state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);

// create machine-readable description of automaton
// (this format is compatible to the libAMoRE++ format.
// NOTE: it does not store if the automaton is deterministic or not!)
basic_string<int32_t> serialize_automaton(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);


// check if given automaton is deterministic
bool is_deterministic(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);

// close state-set under all epsilon-transitions (transition-label == -1)
void epsilon_closure(set<int> & states, multimap<pair<int, int>, int> & transitions);

// simulate a single transition on automaton.
// changes <current> to states reached after run. returns true iff accepting state was reached.
// this version is not very efficient! if you need something efficient, use libAMoRE++.
bool simulate_automaton(set<int> & current, int label,   set<int> & final, multimap<pair<int, int>, int> & transitions, bool check_epsilon_transitions);

// simulate a run on automaton.
// changes <current> to states reached after run. returns true iff accepting state was reached.
// this version is not very efficient! if you need something efficient, use libAMoRE++.
bool simulate_automaton(set<int> & current, list<int>::iterator word_begin, list<int>::iterator word_end,   set<int> & final, multimap<pair<int, int>, int> & transitions);


}; // end namespace libalf

#endif

