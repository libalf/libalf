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
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
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
using namespace amore;

int main(int argc, char**argv)
{
	finite_automaton *model;
	knowledgebase<bool> knowledge;
	ostream_logger log(&cerr, LOGGER_DEBUG);

	bool regex_ok;
	if(argc == 3) {
		model = new nondeterministic_finite_automaton(atoi(argv[1]), argv[2], regex_ok);
		if(!regex_ok) {
			log(LOGGER_ERROR, "bad REGEX. please stick to AMoRE RegEx syntax.\n");
			return 1;
		}
	} else {
		log(LOGGER_ERROR, "please give <alphabet-size> <AMoRE-regular-expression> as parameters.\n");
		return 1;
	}

	int alphabet_size = model->get_alphabet_size();

	// create angluin_simple_table
	angluin_simple_table<bool> ot(&knowledge, &log, alphabet_size);

	conjecture * cj;
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

		// tell algorithm about counterexample
		ot.add_counterexample(counterexample);
		delete cj;
	}

	cout << cj->write(); // dump conjecture

	delete cj;
	delete model;

	return 0;
}

