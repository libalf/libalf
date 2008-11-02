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


int main()
{
	regex r;
	nfa nfa_p;
	dfa dfa_p;

	deterministic_finite_amore_automaton *atm;
	deterministic_finite_amore_automaton hypothesis;
	logger *log;
	teacher<bool> *teach;
	simple_observationtable<bool> *ot;
	oracle_automaton o;

	char filename[128];
	int iteration;

	// init AMoRE buffers
	initbuf();

	log = new ostream_logger(&cout, LOGGER_DEBUG, true);

	// create automaton from regex
#define ALPHABET_SIZE 2
	// asize 2:
	//r = rexFromString(ALPHABET_SIZE, "((a((aa)a))U(((bb))*((((bU(ab))U(bUa)))*)*))"); // mindfa has 1 state.
	//r = rexFromString(ALPHABET_SIZE, "(((bU((aa)U(aUb)))U(a(aUb)))U((aUa)(bb)))"); // mindfa has 4 states.
	r = rexFromString(ALPHABET_SIZE, "(((aa)(a)*)(((a((b(b)*)(aUb)))((ba))*))*)"); // mindfa has 10 states.
	// asize 3:
	//r = rexFromString(ALPHABET_SIZE, "(cbb(ab(c)*))* U (a((cbb*) U a+b+bc)+)");

	if(!r) {
		printf("regex failed\n");
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
	o.set_automaton(*atm);

	// create simple_observationtable and teach it the automaton
	ot = new simple_observationtable<bool>(teach, log, ALPHABET_SIZE);
	cout << "simple_observationtable ok\n";

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

		// once an automaton is generated, test for equality with oracle_automaton
		// if this test is ok, all worked well

		pair<bool, list< list<int> > > oracle_answer;
		oracle_answer = o.equality_query(hypothesis);

		if(oracle_answer.first) {
			cout << "success.\n";
			break;
		}

		snprintf(filename, 128, "counterexample%2d.angluin", iteration);
		file.open(filename);
		cout << "counter example: .";
		print_word(cout, oracle_answer.second.front());
		print_word(file, oracle_answer.second.front());
		cout << "\n";
		ot->add_counterexample(oracle_answer.second.front());
		file.close();
	}

	delete ot;
	delete teach;
	delete log;
	delete atm;

	// release AMoRE buffers
	freebuf();

	return 0;
}

