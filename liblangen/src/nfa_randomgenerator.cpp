/* $Id$
 * vim: fdm=marker
 *
 * liblangen (LANguageGENerator)
 * nfa_randomgenerator: random generator for NFA
 * original idea of algorithm is described in section 4.2 of
 * "F. Denis, A. Lemay and A. Terlutte - Learning regular languages using RFSAs"
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <liblangen/nfa_randomgenerator.h>
#include <liblangen/prng.h>


namespace liblangen {

using namespace std;
using namespace liblangen::prng;

nfa_randomgenerator::nfa_randomgenerator()
{{{
	seed_prng();
}}}

bool nfa_randomgenerator::generate(int alphabet_size, int state_count, int transitions_p_state, float p_initial, float p_final,
	      bool &t_is_dfa, int &t_alphabet_size, int &t_state_count, std::set<int> &t_initial, std::set<int> &t_final, multimap<pair<int,int>, int> &t_transitions)
{
	float x;

	t_initial.clear();
	t_final.clear();
	t_transitions.clear();

	for(int i = 0; i < state_count; i++) {
		// transitions
		for(int t = 0; t < transitions_p_state; ++t) {
			pair<pair<int, int>, int> tr;
			tr.first.first = i;
			tr.first.second = random_int(alphabet_size);
			tr.second = random_int(state_count);
			t_transitions.insert(tr);
		}
		// initial states
		x = random_float();
		if(x < p_initial)
			t_initial.insert(i);
		// final states
		x = random_float();
		if(x < p_final)
			t_final.insert(i);
	}

	t_is_dfa = false;

	t_alphabet_size = alphabet_size;
	t_state_count = state_count;

	return true;
}

}; // end of namespace liblangen

