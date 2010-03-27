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
 * nfa_randomgenerator: testsuite for NFA random generator.
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <liblangen/nfa_randomgenerator.h>
#include <amore++/finite_automaton.h>

using namespace liblangen;
using namespace std;

int main(int argc, char**argv)
{
	nfa_randomgenerator rag;

	int alphabet_size, state_count, tr_per_state;

	float p_initial, p_final;

	if(argc != 6) {
		cout << "please give <alphabet size> <state count> <transitions per state> <p(initial)> <p(final)> as parameters\n";
		return 1;
	}

	alphabet_size = atoi(argv[1]);
	state_count = atoi(argv[2]);
	tr_per_state = atoi(argv[3]);
	p_initial = atof(argv[4]);
	p_final = atof(argv[5]);

	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	set<int> f_initial, f_final;
	multimap<pair<int, int>, int> f_transitions;

	if(!rag.generate(alphabet_size, state_count, tr_per_state, p_initial, p_final,
			f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
		cout << "generator return false. bad parameters?\n";
		return 1;
	}

	amore::finite_automaton * nfa = amore::construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
	if(!nfa) {
		printf("construction failed\n");
		return -1;
	}

	ofstream file;

	file.open("random-nfa.dot");
	file << nfa->visualize();
	file.close();

	amore::finite_automaton * dfa = nfa->determinize();
	dfa->minimize();

	file.open("random-dfa.dot");
	file << dfa->visualize();
	file.close();

	printf("automaton generated: asize %2d, states %2d, mDFA states %2d\n",
			alphabet_size, nfa->get_state_count(), dfa->get_state_count());

	delete dfa;
	delete nfa;

	return 0;
}

