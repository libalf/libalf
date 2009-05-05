/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * NFArandomgenerator: testsuite for NFA random generator.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <LanguageGenerator/NFArandomgenerator.h>
#include <amore++/finite_automaton.h>

using namespace LanguageGenerator;
using namespace amore;
using namespace std;

int main(int argc, char**argv)
{
	NFArandomgenerator rag;

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

	finite_automaton * nfa = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
	if(!nfa) {
		printf("construction failed\n");
		return -1;
	}

	ofstream file;

	file.open("random-nfa.dot");
	file << nfa->generate_dotfile();
	file.close();

	finite_automaton * dfa = nfa->determinize();
	dfa->minimize();

	file.open("random-dfa.dot");
	file << dfa->generate_dotfile();
	file.close();

	printf("automaton generated: asize %2d, states %2d, mDFA states %2d\n",
			alphabet_size, nfa->get_state_count(), dfa->get_state_count());

	delete dfa;
	delete nfa;

	return 0;
}

