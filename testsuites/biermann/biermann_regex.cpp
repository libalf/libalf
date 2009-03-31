/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <stdlib.h> // for PRNG

#include "libalf/alf.h"

#include <libalf/automata_amore.h>
#include <libalf/algorithm_biermann_minisat.h>

#include <amore/vars.h>
#include <amore/rexFromString.h>
#include <amore/rex2nfa.h>

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;




// add knowledge abount epsilon?
bool add_epsilon = true;
// how many runs should be used to generate knowledge from regex-generated automaton?
int runs = 100;
// how long should these runs be?
int run_length = 8;





int main(int argc, char**argv)
{
	finite_language_automaton *nfa = NULL;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<ANSWERTYPE> knowledge;

	ofstream file;
	char filename[128];

	int alphabet_size;

	// init AMoRE buffers
	initbuf(); // XXX LEAK


	bool regex_ok;

	if(argc == 3) {
		nfa = new nondeterministic_finite_amore_automaton(atoi(argv[1]), argv[2], regex_ok); // XXX LEAK
	} else {
		if(argc == 2) {
			nfa = new nondeterministic_finite_amore_automaton(argv[1], regex_ok); // XXX LEAK
		} else {
			cout << "either give a sole regex as parameter, or give <alphabet size> <regex>.\n\n";
			cout << "example regular expressions:\n";
			cout << "alphabet size, \"regex\":\n";
			cout << "2 '((a((aa)a))U(((bb))*((((bU(ab))U(bUa)))*)*))'\n";
			cout << "2 '(((bb)|a)(b(((bb)b)(((aa)a)|a))))'\n";
			cout << "2 '(((aa)(a)*)(((a((b(b)*)(aUb)))((ba))*))*)'\n";
			cout << "3 '(cbb(ab(c)*))* U (a((cbb*) U a+b+bc)+)'\n";
			return 1;
		}
	}

	if(regex_ok) {
		log(LOGGER_INFO, "regex ok.\n");
	} else {
		log(LOGGER_ERROR, "regex failed.\n");
		return 1;
	}

	alphabet_size = nfa->get_alphabet_size();

	printf("alphabet size set to %d\n", alphabet_size);

	file.open("original-nfa.dot");
	file << nfa->generate_dotfile();
	file.close();

	srand(time(NULL));

	list<int> w;
	if(add_epsilon)
		knowledge.add_knowledge(w, nfa->contains(w));

	// create sample set in knowledgebase
	for(int i = 0; i < runs; i++) {
		w.clear();
		for(int j = 0; j < run_length; j++) {
			unsigned long int r = rand();
			r = (r * alphabet_size) / RAND_MAX;
			w.push_back(r);
			knowledge.add_knowledge(w, nfa->contains(w));
		}
	}

	cout << "\n";
	knowledge.print(cout);
	cout << "\n";

	if(nfa)
		delete nfa; // not needed anymore

	MiniSat_biermann<ANSWERTYPE> diebels(&knowledge, &log, alphabet_size);

	deterministic_finite_amore_automaton hypothesis;

	if(!diebels.advance(&hypothesis))
		printf("\nadvance returned false\n\n");

	printf("\n\n");
//	diebels.print(cout);

	snprintf(filename, 128, "hypothesis.dot");
	file.open(filename);
	file << hypothesis.generate_dotfile();
	file.close();

	// release AMoRE buffers
	freebuf();

	return 0;
}

