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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

/*
 * dfa_enumerator: testsuite for DFA enumerator.
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <liblangen/dfa_enumerator.h>
#include <libalf/automaton.h>

using namespace liblangen;
using namespace libalf;
using namespace std;

int main(int argc, char**argv)
{
	int alphabet_size;
	int state_count;

	if(argc != 3) {
		cout << "please give <alphabet size> and <state count> as parameters\n";
		return 1;
	}

	alphabet_size = atoi(argv[1]);
	state_count = atoi(argv[2]);

	dfa_enumerator denum(state_count, alphabet_size);

	// generate random automaton and store it
	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	set<int> f_initial, f_final;
	multimap<pair<int, int>, int> f_transitions;

	int id = 0;

	while(!denum.generation_completed()) {
		denum.derive(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		ofstream file;
		char filename[64];
		snprintf(filename, 64, "dfa_%010d.dot", id);
		file.open(filename);
		file << automaton2dotfile(f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		file.close();

		denum.next(true);

		id++;
	}

	return 0;
}

