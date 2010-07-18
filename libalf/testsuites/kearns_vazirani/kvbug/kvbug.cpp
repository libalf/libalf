/* $Id: learn_regex.cpp 1119 2009-12-18 21:19:54Z davidpiegdon $
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

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <libalf/alf.h>
#include <libalf/algorithm_kearns_vazirani.h>

#include <amore++/deterministic_finite_automaton.h>

#include "amore_alf_glue.h"

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;

int main()
{
	amore::finite_automaton *dfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<ANSWERTYPE> knowledge;

	bool success = false;

	int alphabet_size;

	basic_string<int32_t> str;
	basic_string<int32_t>::const_iterator si;
	if(!file_to_basic_string("model.ser", str)) {
		cout << "failed to load file \"model.ser\".\n";
		return -1;
	}
	dfa = new amore::deterministic_finite_automaton;
	si = str.begin();
	if(!dfa->deserialize(si, str.end())) {
		cout << "failed to deserialize automaton\n.";
		return -1;
	}
	if(si != str.end()) {
		cout << "garbage at end of file? trying to ignore.\n";
		cout << "garbage: { ";
		while(si != str.end()) {
			cout << ntohl(*si) << "; ";
			++si;
		}
		cout << "};\n";
	}

	alphabet_size = dfa->get_alphabet_size();

	// create kearns/vazirani learning algorithm and teach it the automaton
	kearns_vazirani<ANSWERTYPE> ot(&knowledge, &log, alphabet_size, true);
	amore::finite_automaton * hypothesis = NULL;

	while(!success) {
		conjecture * cj = NULL;

		while( NULL == (cj = ot.advance()) ) {
			printf("queries...\n");
			int d = amore_alf_glue::automaton_answer_knowledgebase(*dfa, knowledge);
			printf("done. (%d)\n", d);
		}

		if(hypothesis)
			delete hypothesis;

		libalf::finite_automaton * ba = dynamic_cast<libalf::finite_automaton*>(cj);
		set<int> final_states;
		ba->get_final_states(final_states);
		hypothesis = amore::construct_amore_automaton(ba->is_deterministic, ba->input_alphabet_size, ba->state_count, ba->initial_states, final_states, ba->transitions);
		delete cj;

		if(!hypothesis) {
			printf("generation of hypothesis failed!\n");
			return -1;
		}

		printf("eq query...\n");
		list<int> counterexample;
		if(amore_alf_glue::automaton_antichain_equivalence_query(*dfa, *hypothesis, counterexample)) {
			// equivalent
			success = true;
			break;
		}
		ot.add_counterexample(counterexample);

	}

	delete dfa;
	delete hypothesis;

	if(success)
		return 0;
	else
		return 2;
}

