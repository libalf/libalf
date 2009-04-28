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

#include <iostream>
#include <fstream>

#include <gmpxx.h>

#include <LanguageGenerator/DFArandomgenerator.h>

#include "amore_langen_glue.h"

using namespace LanguageGenerator;
using namespace std;

int main(int argc, char**argv)
{
	DFArandomgenerator rag;

	int alphabet_size;
	int state_count;

	mpz_class c;

	list<int> K;
	list<int>::iterator ki;

	if(argc != 3) {
		cout << "please give <alphabet size> and <state count> as parameters\n";
		return 1;
	}

	alphabet_size = atoi(argv[1]);
	state_count = atoi(argv[2]);

	// generate random automaton and store it
	amore_langen_glue::amore_automaton_holder automaton;

	if(!rag.generate(alphabet_size, state_count, automaton)) {
		cout << "generator returned false. bad parameters?\n";
		return 1;
	}

	ofstream file;

	file.open("random-f.dot");
	file << automaton.get_automaton()->generate_dotfile();
	file.close();

	automaton.get_automaton()->minimize();

	file.open("random-m.dot");
	file << automaton.get_automaton()->generate_dotfile();
	file.close();

	printf("alphabet size %2d state count %2d mdfa size %2d\n", alphabet_size, state_count, automaton.get_automaton()->get_state_count());

	return 0;
}

