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
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

/*
 * dfa_randomgenerator: testsuite for DFA random generator.
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <liblangen/dfa_randomgenerator.h>
#include <amore++/finite_automaton.h>

using namespace liblangen;
using namespace amore;
using namespace std;

int main(int argc, char**argv)
{
	dfa_randomgenerator rag;

	int alphabet_size;
	int state_count;

	if(argc != 3) {
		cout << "please give <alphabet size> and <state count> as parameters\n";
		return 1;
	}

	alphabet_size = atoi(argv[1]);
	state_count = atoi(argv[2]);

	// generate random automaton and store it
	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	set<int> f_initial, f_final;
	multimap<pair<int, int>, int> f_transitions;

	if(!rag.generate(alphabet_size, state_count,
			 f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
		cout << "generator returned false. bad parameters?\n";
		return 1;
	}

	finite_automaton * dfa = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
	if(!dfa) {
		printf("construction failed\n");
		return -1;
	}

	ofstream file;

	file.open("random-f.dot");
	file << dfa->generate_dotfile();
	file.close();

	dfa->minimize();

	file.open("random-m.dot");
	file << dfa->generate_dotfile();
	file.close();

	printf("alphabet size %2d state count %2d mdfa size %2d\n", alphabet_size, state_count, dfa->get_state_count());

	delete dfa;

	return 0;
}

