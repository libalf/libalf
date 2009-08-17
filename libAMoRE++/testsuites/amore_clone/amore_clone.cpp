/* $Id$
 * vim: fdm=marker
 *
 * amore++
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

#include "amore++/nondeterministic_finite_automaton.h"
#include "amore++/deterministic_finite_automaton.h"

using namespace std;
using namespace amore;

int main(int argc, char**argv)
{
	finite_automaton *nfa;
	finite_automaton *dfa;

	basic_string<int32_t> serialized;
	basic_string<int32_t>::iterator sit;
	ofstream file;

	bool regex_ok;
	if(argc == 3) {
		nfa = new nondeterministic_finite_automaton(atoi(argv[1]), argv[2], regex_ok);
	} else {
		if(argc == 2) {
			nfa = new nondeterministic_finite_automaton(argv[1], regex_ok);
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
		printf("REGEX ok.\n");
	} else {
		printf("REGEX failed.\n");
		return 1;
	}

	dfa = nfa->determinize();

	finite_automaton *c, *d;

	c = nfa->clone();
	d = dfa->clone();

	delete c;	// XXX
	delete d;

	list<int> w;
	bool is_empty;
	w = nfa->get_sample_word(is_empty);	// XXX
	w = dfa->get_sample_word(is_empty);

	return 0;
}

