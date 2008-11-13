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

#include "libalf/alf.h"

#include "libalf/automata_amore.h"

#include <amore/vars.h>

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	statistics stats;

	finite_language_automaton *nfa;
	finite_language_automaton *dfa;
	logger *log;

	basic_string<int32_t> serialized;
	ofstream file;

	bool success = false;

	// init AMoRE buffers
	initbuf();

	// init logger
	log = new ostream_logger(&cout, LOGGER_DEBUG);

	bool regex_ok;
	if(argc == 3) {
		nfa = new nondeterministic_finite_amore_automaton(atoi(argv[1]), argv[2], regex_ok);
	} else {
		if(argc == 2) {
			nfa = new nondeterministic_finite_amore_automaton(argv[1], regex_ok);
		} else {
			cout << "either give a sole regex as parameter, or give <alphabet size> <regex>.\n\n";
			cout << "example regular expressions:\n";
			cout << "alphabet size, \"regex\":\n";
			cout << "2 '((a((aa)a))U(((bb))*((((bU(ab))U(bUa)))*)*))'\n";
			cout << "2 '(((bU((aa)U(aUb)))U(a(aUb)))U((aUa)(bb)))'\n";
			cout << "2 '(((aa)(a)*)(((a((b(b)*)(aUb)))((ba))*))*)'\n";
			cout << "3 '(cbb(ab(c)*))* U (a((cbb*) U a+b+bc)+)'\n";
			return 1;
		}
	}

	if(regex_ok) {
		(*log)(LOGGER_INFO, "regex ok.\n");
	} else {
		(*log)(LOGGER_ERROR, "regex failed.\n");
		return 1;
	}

	file.open("original-nfa.dot");
	file << nfa->generate_dotfile();
	file.close();

	dfa = nfa->determinize();
	dfa->minimize();

	file.open("original-dfa.dot");
	file << dfa->generate_dotfile();
	file.close();



	// serialize and deserialize now
	serialized = nfa->serialize();

	delete nfa;
	nfa = new nondeterministic_finite_amore_automaton();
	nfa->deserialize(serialized);

	file.open("deserialized-nfa.dot");
	file << nfa->generate_dotfile();
	file.close();


	serialized = dfa->serialize();

	delete dfa;
	dfa = new deterministic_finite_amore_automaton();
	dfa->deserialize(serialized);

	file.open("deserialized-dfa.dot");
	file << dfa->generate_dotfile();
	file.close();



	delete log;
	delete nfa;
	delete dfa;

	// release AMoRE buffers
	freebuf();

	if(success)
		return 0;
	else
		return 2;
}

