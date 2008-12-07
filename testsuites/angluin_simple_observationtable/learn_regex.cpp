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
#include <amore/rexFromString.h>
#include <amore/rex2nfa.h>

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	statistics stats;

	finite_language_automaton *nfa;
	ostream_logger log(&cout, LOGGER_DEBUG);
	teacher_automaton<ANSWERTYPE> teach;
	teacher<ANSWERTYPE> * teacher_p;
	oracle_automaton o;

	char filename[128];
	ofstream file;

	int iteration;
	bool success = false;

	int alphabet_size;

	// use NFA or DFA as oracle/teacher source?
	bool use_nfa = false;

	// use structured query tree or directly give teacher to algorithm?
	bool use_sqt = false;

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

	file.open("original-nfa.dot");
	file << nfa->generate_dotfile();
	file.close();

	if(use_nfa) {
		teach.set_automaton(*nfa);
		o.set_automaton(*nfa);
	} else {
		finite_language_automaton *dfa;

		dfa = nfa->determinize();
		dfa->minimize(); // XXX LEAK

		file.open("original-dfa.dot");
		file << dfa->generate_dotfile();
		file.close();

		teach.set_automaton(*dfa); // XXX LEAK
		o.set_automaton(*dfa);

		delete dfa;
	};

	delete nfa; // not needed anymore

	// create oracle instance and teacher instance
	teach.set_statistics_counter(&stats);
	o.set_statistics_counter(&stats);

	// create angluin_simple_observationtable and teach it the automaton
	if(use_sqt)
		teacher_p = NULL;
	else
		teacher_p = &teach;
	angluin_simple_observationtable<ANSWERTYPE> ot(teacher_p, &log, alphabet_size);
	deterministic_finite_amore_automaton hypothesis;

	for(iteration = 1; iteration <= 100; iteration++) {
		structured_query_tree<ANSWERTYPE> * sqt;
		while( (sqt = ot.derive_hypothesis(&hypothesis)) != NULL) {
			// resolve SQT
			teach.answer_structured_query(*sqt);
			// apply SQT
			ot.learn_from_structured_query(*sqt);

			delete sqt;
		}

		snprintf(filename, 128, "observationtable%2d.text.angluin", iteration);
		file.open(filename);
		ot.print(file);
		file.close();

		{
			snprintf(filename, 128, "observationtable%2d.serialized.angluin", iteration);
			file.open(filename);

			basic_string<int32_t> serialized;
			basic_string<int32_t>::iterator it;

			serialized = ot.serialize();

			for(it = serialized.begin(); it != serialized.end(); it++) {
				file << ntohl(*it);
				file << ";";
			}

			file.close();
		}

		snprintf(filename, 128, "hypothesis%2d.dot", iteration);
		file.open(filename);
		file << hypothesis.generate_dotfile();
		file.close();

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		pair<bool, list< list<int> > > oracle_answer;
		oracle_answer = o.equivalence_query(hypothesis); // XXX LEAK

		if(oracle_answer.first) {
			cout << "success.\n";
			success = true;
			break;
		}

		snprintf(filename, 128, "counterexample%2d.angluin", iteration);
		file.open(filename);
		print_word(file, oracle_answer.second.front());
		ot.add_counterexample(oracle_answer.second.front());
		file.close();
	}

	cout << "required membership queries: " << stats.query_count.membership << "\n";
	cout << "required equivalence queries: " << stats.query_count.equivalence << "\n";
	cout << "minimal state count: " << hypothesis.get_state_count() << "\n";
	// cout << "difference to original state count: " << dfa->get_state_count() - hypothesis.get_state_count() << "\n";

	// release AMoRE buffers
	freebuf();

	if(success)
		return 0;
	else
		return 2;
}

