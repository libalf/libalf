/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * DFAenumerator: testsuite for DFA enumerator.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <LanguageGenerator/DFAenumerator.h>
#include <libalf/automaton.h>

using namespace LanguageGenerator;
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

	DFAenumerator denum(state_count, alphabet_size);

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

