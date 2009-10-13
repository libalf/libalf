/* $Id$
 * vim: fdm=marker
 *
 * This file is part of liblangen (LANguageGENerator)
 *
 * liblangen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblangen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with liblangen.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

/*
 * nfa_randomgenerator: random generator for NFA
 * original idea of algorithm is described in section 4.2 of
 * "F. Denis, A. Lemay and A. Terlutte - Learning regular languages using RFSAs"
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

