/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

/*
 * this is a simple example for usage of an online-algorithm (angluin)
 */

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <libalf/alf.h>
#include <libalf/algorithm_angluin.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	amore::finite_automaton *model;		// this is an automaton that we use both as oracle and teacher
	knowledgebase<bool> knowledge;		// data storage for all queries and answered queries
	ostream_logger log(&cerr, LOGGER_DEBUG);

	// first, get a regular expression from the command-line and construct the model from it.
	// for this, we use libAMoRE++.
	bool regex_ok;
	if(argc == 3) {
		amore::finite_automaton *nfa;
		nfa = new amore::nondeterministic_finite_automaton(atoi(argv[1]), argv[2], regex_ok);
		model = nfa->determinize();
		delete nfa;
		if(!regex_ok) {
			log(LOGGER_ERROR, "bad REGEX. please stick to AMoRE RegEx syntax.\n");
			return 1;
		}
	} else {
		log(LOGGER_ERROR, "please give <alphabet-size> <AMoRE-regular-expression> as parameters.\n");
		return 1;
	}

	int alphabet_size = model->get_alphabet_size();

	// create learning algorithm for finite automata:
	//   angluin_simple_table over boolean output alphabet (a state can accept=true or reject=false)
	angluin_simple_table<bool> ot(&knowledge, &log, alphabet_size);

	conjecture * cj; // storage for the conjetures we get from the algorithm
	list<int> counterexample;

	// this is the main learning loop:
	while(1) {
		// until a conjecture is ready, answer all queries from model (via libAMoRE++)
		while( NULL == (cj = ot.advance()) )
			amore_alf_glue::automaton_answer_knowledgebase(*model, knowledge);

		// conjecture is ready. do an equivalence query.
		// get counterexample if model and conjecture are different (via libAMoRE++)
		if(amore_alf_glue::automaton_equivalence_query(*model, cj, counterexample))
			break; // they are equal (according to libAMoRE++)

		// otherwise, tell algorithm the counterexample
		ot.add_counterexample(counterexample);
		delete cj;
	}

	cout << cj->write(); // dump conjecture

	delete cj;
	delete model;

	return 0;
}

