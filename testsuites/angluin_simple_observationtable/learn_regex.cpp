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
#include <fstream>

#include "libalf/alf.h"

#include "libalf/automata_amore.h"

#include <amore/vars.h>
#include <amore/rexFromString.h>
#include <amore/rex2nfa.h>
#include <amore/nfa2dfa.h>

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	regex r;
	nfa nfa_p;
	dfa dfa_p;

	statistics stats;

	deterministic_finite_amore_automaton *atm;
	deterministic_finite_amore_automaton hypothesis;
	logger *log;
	teacher<bool> *teach;
	angluin_simple_observationtable<bool> *ot;
	oracle_automaton o;

	char filename[128];
	int iteration;

	int alphabet_size;
	char *regex;
	char *p;

	if(argc == 3) {
		alphabet_size = atoi(argv[1]);
		if(alphabet_size <= 0) {
			cout << "insane or invalid alphabet_size\n";
			return 1;
		}

		regex = argv[2];
	} else {
		if(argc == 2) {
			regex = argv[1];
			// get alphabet_size
			p = regex;
			char c = 'a';
			while(*p) {
				if(*p > 'a' && *p < 'z' && *p > c)
					c = *p;
				p++;
			}
			alphabet_size = 1 + c - 'a';

			cout << "choosing " << alphabet_size << " as alphabet sizen\n";
		} else {
			cout << "please give the 2 parameters <alphabet size> <regex>\n\n";
			cout << "example regular expressions:\n";
			cout << "alphabet size, \"regex\":\n";
			cout << "2 '((a((aa)a))U(((bb))*((((bU(ab))U(bUa)))*)*))'\n";
			cout << "2 '(((bU((aa)U(aUb)))U(a(aUb)))U((aUa)(bb)))'\n";
			cout << "2 '(((aa)(a)*)(((a((b(b)*)(aUb)))((ba))*))*)'\n";
			cout << "3 '(cbb(ab(c)*))* U (a((cbb*) U a+b+bc)+)'\n";
			return 1;
		}
	}

	// translate pipes ('|') into 'U', for amore syntax
	p = regex;
	while(*p) {
		if(*p == '|')
			*p = 'U';
		p++;
	}



	// init AMoRE buffers
	initbuf();

	log = new ostream_logger(&cout, LOGGER_DEBUG);

	// create automaton from regex
	cout << "alphabet size: " << alphabet_size <<", regex: " << regex << "\n";
	r = rexFromString(alphabet_size, regex);

	if(!r) {
		cout << "regex failed\n";
		return 1;
	}
	nfa_p = rex2nfa(r);
	dfa_p = nfa2dfa(nfa_p);

	freenfa(nfa_p);
	freerex(r);

	atm = new deterministic_finite_amore_automaton(dfa_p);

	ofstream file;
	file.open("original.dot");
	file << atm->generate_dotfile();
	file.close();

	// create oracle instance and teacher instance
	teach = new teacher_automaton<bool>(atm);
	teach->set_statistics_counter(&stats);
	o.set_automaton(*atm);
	o.set_statistics_counter(&stats);

	// create angluin_simple_observationtable and teach it the automaton
	ot = new angluin_simple_observationtable<bool>(teach, log, alphabet_size);
	cout << "angluin_simple_observationtable ok\n";

	for(iteration = 1; iteration <= 20; iteration++) {
		cout << "iteration " << iteration <<":\n";
		ot->derive_hypothesis(&hypothesis);

		snprintf(filename, 128, "observationtable%2d.angluin", iteration);
		file.open(filename);
		ot->print(file);
		file.close();

		snprintf(filename, 128, "hypothesis%2d.dot", iteration);
		file.open(filename);
		file << hypothesis.generate_dotfile();
		file.close();

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		pair<bool, list< list<int> > > oracle_answer;
		oracle_answer = o.equivalence_query(hypothesis);

		if(oracle_answer.first) {
			cout << "success.\n";
			break;
		}

		snprintf(filename, 128, "counterexample%2d.angluin", iteration);
		file.open(filename);
		cout << "counter example: ";
		print_word(cout, oracle_answer.second.front());
		print_word(file, oracle_answer.second.front());
		cout << "\n";
		ot->add_counterexample(oracle_answer.second.front());
		file.close();
	}

	cout << "required membership queries: " << stats.query_count.membership << "\n";
	cout << "required equivalence queries: " << stats.query_count.equivalence << "\n";

	delete ot;
	delete teach;
	delete log;
	delete atm;

	// release AMoRE buffers
	freebuf();

	return 0;
}

