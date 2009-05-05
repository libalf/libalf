/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * DFArandomgenerator: testsuite for DFA random generator.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <LanguageGenerator/DFArandomgenerator.h>
#include <amore++/finite_automaton.h>

using namespace LanguageGenerator;
using namespace amore;
using namespace std;

int main(int argc, char**argv)
{
	DFArandomgenerator rag;

	int alphabet_size;
	int state_count;

	list<int> K;
	list<int>::iterator ki;

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
	finite_automaton * automaton = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);

	if(!automaton) {
		printf("construction failed\n");
		return -1;
	}

	ofstream file;

	file.open("random-f.dot");
	file << automaton->generate_dotfile();
	file.close();

	automaton->minimize();

	file.open("random-m.dot");
	file << automaton->generate_dotfile();
	file.close();

	printf("alphabet size %2d state count %2d mdfa size %2d\n", alphabet_size, state_count, automaton->get_state_count());

	delete automaton;

	return 0;
}

