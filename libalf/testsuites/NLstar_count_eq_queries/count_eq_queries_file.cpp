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

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <libalf/alf.h>
#include <libalf/algorithm_NLstar.h>
#include <libalf/basic_string.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	statistics stats;

	amore::finite_automaton *nfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<ANSWERTYPE> knowledge;

//	char filename[128];
//	ofstream file;
	int mindfa_statecount;

	int iteration;
	bool success = false;

	int alphabet_size;

	{{{ // get automaton from file
		if(argc != 2) {
			cout << "please give filename as sole parameter.\n";
			return -1;
		};
		basic_string<int32_t> str;
		basic_string<int32_t>::const_iterator si;
		if(!file_to_basic_string(argv[1], str)) {
			cout << "failed to load file \"" << argv[1] << "\".\n";
			return -1;
		}
		nfa = new amore::nondeterministic_finite_automaton;
		si = str.begin();
		if(!nfa->deserialize(si, str.end())) {
			cout << "failed to deserialize automaton\n.";
			return -1;
		}
		if(si != str.end())
			cout << "garbage at end of file? trying to ignore.\n";
	}}}

	alphabet_size = nfa->get_alphabet_size();

	{{{ /* dump original automata */
//		file.open("original-nfa.dot"); file << nfa->visualize(); file.close();

		amore::finite_automaton * dfa;
		dfa = nfa->determinize();
		dfa->minimize();
		mindfa_statecount = dfa->get_state_count();
//		file.open("original-dfa.dot"); file << dfa->visualize(); file.close();
		delete dfa;
	}}}


	// create NLstar table and teach it the automaton
	NLstar_table<ANSWERTYPE> ot(&knowledge, &log, alphabet_size);

	for(iteration = 1; iteration <= 100; iteration++) {
		int c = 'a';
		conjecture *cj;

		while( NULL == (cj = ot.advance()) ) {
			// resolve missing knowledge:
			stats.queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*nfa, knowledge);
			c++;
		}

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		list<int> counterexample;
		stats.queries.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*nfa, cj, counterexample)) {
			// equivalent
			cout << "success.\n";
			success = true;
			break;
		}
		delete cj;

		/*
		snprintf(filename, 128, "counterexample%02d.NLstar", iteration);
		file.open(filename);
		print_word(file, counterexample);
		file.close();
		*/
		ot.add_counterexample(counterexample);
	}

	iteration++;
	/*
	snprintf(filename, 128, "knowledgebase%02d-final.dot", iteration);
	file.open(filename);
	file << knowledge.visualize();
	file.close();
	*/

	stats.memory = ot.get_memory_statistics();
	stats.queries.membership = knowledge.count_resolved_queries();

	int ret = 0;

	if(stats.queries.equivalence > mindfa_statecount) {
		log(LOGGER_WARN, "mDFA size %d but %d eq queries for %s!\n", mindfa_statecount, stats.queries.equivalence, argv[1]);
		ret = 1;
	}

	delete nfa;

	if(success)
		return ret;
	else
		return 2;
}

