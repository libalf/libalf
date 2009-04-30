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

string automaton2dotfile(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);

basic_string<int32_t> serialize_automaton(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);

}; // end namespace libalf

#endif

