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
	simple_observationtable<bool> *ob;

	log = new ostream_logger(&cout, LOGGER_DEBUG, true);

	// create automaton from regex
//	r = rexFromString(2, "(aba*)-(abaaa)");
	r = rexFromString(2, "abb(ab)*abb");
	cout << "regex ok\n";
	nfa_p = rex2nfa(r);
	cout << "nfa ok\n";
	dfa_p = nfa2dfa(nfa_p);
	cout << "dfa ok\n";

	atm = new deterministic_finite_amore_automaton(dfa_p);

	std::string s;
	s = atm->generate_dotfile();
	cout << " --- begin automata\n" << s << " --- end automata\n";
	ofstream dotfile;
	dotfile.open("aso.dot");
	dotfile << s;
	dotfile.close();

	// create oracle instance and teacher instance
	teach = new teacher_automaton<bool>(atm);
	cout << "teacher ok\n";

	// create simple_observationtable and teach it the automaton
	ob = new simple_observationtable<bool>(teach, log, 2);
	cout << "simple_observationtable ok\n";

	ob->derive_hypothesis(&hypothesis);

	cout << hypothesis.generate_dotfile();

	// once an automaton is generated, test for equality with oracle_automaton
	// if this test is ok, all worked well
	oracle_automaton o;
	o.set_automaton(*atm);

//	pair<bool, list< list<int> > > oracle_answer;
//	oracle_answer = o.equality_query(hypothesis);

	delete ob;
	delete teach;
	delete log;
	delete atm;

	return 0;
}

