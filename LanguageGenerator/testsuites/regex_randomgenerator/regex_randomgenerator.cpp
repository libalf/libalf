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

#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/finite_automaton.h>

#include <LanguageGenerator/regex_randomgenerator.h>

using namespace LanguageGenerator;
using namespace amore;
using namespace std;

int main(int argc, char**argv)
{
	int alphabet_size;
	int num_op;
	bool show_mregex = false;

	if(argc != 3 && argc != 4 || (argc == 4 && 0 != strcmp(argv[1], "-r")) ) {
		cout << "please give <alphabet size> and <operand count> as parameters,\n"
			"possibly prefixed by -r to see the regex derived from the mDFA\n";
		return 1;
	}

	if(argc == 4) {
		show_mregex = true;
		alphabet_size = atoi(argv[2]);
		num_op = atoi(argv[3]);
	} else {
		alphabet_size = atoi(argv[1]);
		num_op = atoi(argv[2]);
	}

	string regex;
	regex_randomgenerator rrg;

	float p_sigma[alphabet_size];
	float peps, pcon, puni, pstar;
	for(int i = 0; i < alphabet_size; i++)
		p_sigma[i] = 5./alphabet_size;
	peps = 2;
	pcon = 50;
	puni = 25;
	pstar = 13;

	regex = rrg.generate(num_op, alphabet_size, p_sigma, peps, pcon, puni, pstar);

	if(regex == "") {
		cout << "empty regex\n";
		return 0;
	}

	bool success;
	nondeterministic_finite_automaton automaton(alphabet_size, regex.c_str(), success);

	if(!success) {
		cout << "failed to create automaton from regex! RegEx is: \"" << regex << "\"\n";
		return 1;
	}

	finite_automaton *dfa;
	dfa = automaton.determinize();
	dfa->minimize();

	printf("NFA has %3d, mDFA has %3d states.\n"
		"\t\tRegEx:  %s\n",
		automaton.get_state_count(), dfa->get_state_count(), regex.c_str());

	if(show_mregex) {
		nondeterministic_finite_automaton *nfa;

		nfa = dynamic_cast<nondeterministic_finite_automaton*>(dfa->nondeterminize());
		string minrex = nfa->to_regex();

		printf("\t\tmRegEx: %s\n", minrex.c_str());
		delete nfa;
	}

	delete dfa;

	return 0;
}

