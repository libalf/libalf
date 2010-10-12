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

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

using namespace std;
using namespace libalf;

amore::nondeterministic_finite_automaton * sample_automaton()
{{{
	amore::nondeterministic_finite_automaton * nfa;

	/*
	int automaton[] = {
		0, // is not deterministic
		2, // alphabet size
		3, // state count
		1, // number of initial states
		0,
		1, // number of final states
		0,
		6, // number of transitions
		0,0,0, 0,1,1, 1,0,0, 1,1,2, 2,0,0, 2,1,0
	};
	*/

	// sample automaton from "angluin style learning of NFA"-paper:
	// Fig. 11: (NFA)
//	int automaton[] = { 0, 2, 4, 1, 0, 4, 0,1,2,3, 7, 0,0,1, 0,1,1, 1,0,2, 1,1,2, 2,0,3, 2,1,3, 3,1,0 };

	// Fig. 13: (NFA)
//	int automaton[] = { 0, 2, 6, 1, 0, 5, 0,1,2,3,4, 12, 0,1,1, 0,0,2, 1,1,1, 1,0,3, 2,0,2, 2,1,4, 3,0,5, 3,1,5, 4,1,4, 4,0,5, 5,0,5, 5,1,5 };

	// Fig. 15: (NFA)
//	int automaton[] = { 0, 3, 7, 1, 0, 4, 0,1,4,5, 21, 0,0,1, 0,1,2, 0,2,3, 1,0,1, 1,1,4, 1,2,2, 2,0,5, 2,1,4, 2,2,4, 3,0,5, 3,1,3, 3,2,5, 4,0,1, 4,1,4, 4,2,4, 5,0,6, 5,1,3, 5,2,3, 6,0,5, 6,1,1, 6,2,2 };

	// Fig. 16: (NFA)
//	int automaton[] = { 0, 2, 4, 1, 0, 2, 1,2, 6, 0,1,1, 1,0,2, 1,1,1, 2,1,3, 3,0,2, 3,1,2, };

	// Fig. 18: (NFA)
//	int automaton[] = { 0, 2, 10, 1, 0, 5, 0,1,2,8,9, 12, 0,0,1, 0,1,2, 1,0,3, 1,1,7, 2,1,2, 3,0,4, 4,0,3, 4,1,5, 5,1,6, 6,1,9, 7,1,8, 8,1,9 };

	// Fig. 24: (NFA)
//	int automaton[] = { 0, 2, 6, 1, 0, 2, 3,5, 6, 0,1,1, 1,0,2, 2,0,3, 3,0,5, 3,1,4, 4,0,5 };

	// automaton that will have a huge hypothesis in between, a smaller final (NFA)
//	int automaton[] = { 0, 2, 4, 2,1,3, 1,2, 6, 0,0,1, 1,0,0, 1,1,0, 1,0,2, 3,1,3, 3,0,2 };

	// automaton that has more eq-queries than states
	int automaton[] = { 0, 2, 5, 1, 0, 4, 0, 1, 3, 4, 10, 0,0,0, 0,1,1, 1,0,0, 1,1,2, 2,0,3, 2,1,4, 3,0,0, 3,1,3, 4,0,0, 4,1,0 };

	basic_string<int32_t> serial;
	basic_string<int32_t>::const_iterator si;

	serial += htonl(sizeof(automaton)/sizeof(int));
	for(unsigned int i = 0; i < sizeof(automaton)/sizeof(int); i++) {
		serial += htonl(automaton[i]);
	}

	si = serial.begin();
	nfa = new amore::nondeterministic_finite_automaton;
	nfa->deserialize(si, serial.end());

	return nfa;
}}}

int main(int argc, char**argv)
{
	statistics stats;

	amore::finite_automaton *nfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<bool> knowledge;

	char filename[128];
	ofstream file;
	int mindfa_statecount;

	int iteration;
	bool success = false;

	int alphabet_size;

	nfa = sample_automaton();

	alphabet_size = nfa->get_alphabet_size();

	{{{ /* dump original automata */
		file.open("original-nfa.dot"); file << nfa->visualize(); file.close();

		amore::finite_automaton * dfa;
		dfa = nfa->determinize();
		dfa->minimize();
		mindfa_statecount = dfa->get_state_count();
		file.open("original-dfa.dot"); file << dfa->visualize(); file.close();
		delete dfa;
	}}}


	// create NLstar table and teach it the automaton
	NLstar_table<bool> ot(&knowledge, &log, alphabet_size);
	amore::finite_automaton * hypothesis = NULL;

	for(iteration = 1; iteration <= 100; iteration++) {
		int c = 'a';
		conjecture * cj;

		while( NULL == (cj = ot.advance()) ) {
			// resolve missing knowledge:

			snprintf(filename, 128, "knowledgebase%02d%c.dot", iteration, c);
			file.open(filename); file << knowledge.visualize(); file.close();

			// create query-tree
			knowledgebase<bool> * query;
			query = knowledge.create_query_tree();

			snprintf(filename, 128, "knowledgebase%02d%c-q.dot", iteration, c);
			file.open(filename); file << query->visualize(); file.close();

			// answer queries
			stats.queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*nfa, *query);

			snprintf(filename, 128, "knowledgebase%02d%c-r.dot", iteration, c);
			file.open(filename); file << query->visualize(); file.close();

			// merge answers into knowledgebase
			knowledge.merge_knowledgebase(*query);
			delete query;
			c++;
		}

		libalf::finite_automaton * ba = dynamic_cast<libalf::finite_automaton*>(cj);
		if(hypothesis)
			delete hypothesis;
		hypothesis = amore_alf_glue::automaton_libalf2amore(*ba);
		delete cj;
		if(!hypothesis) {
			printf("generation of hypothesis failed!\n");
			return -1;
		}

		{{{ /* dump/serialize table */
			basic_string<int32_t> serialized;
			basic_string<int32_t>::const_iterator it;

			snprintf(filename, 128, "table%02d.text.NLstar", iteration);
			file.open(filename); ot.print(file); file.close();

			/*
			serialized = ot.serialize();

			snprintf(filename, 128, "table%02d.serialized.NLstar", iteration);
			file.open(filename);

			for(it = serialized.begin(); it != serialized.end(); it++) {
				file << ntohl(*it);
				file << ";";
			}

			file.close();
			*/
		}}}

		snprintf(filename, 128, "hypothesis%02d.dot", iteration);
		file.open(filename); file << hypothesis->visualize(); file.close();

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		list<int> counterexample;
		stats.queries.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*nfa, *hypothesis, counterexample)) {
			// equivalent
			cout << "success.\n";
			success = true;
			break;
		}

		snprintf(filename, 128, "counterexample%02d.NLstar", iteration);
		file.open(filename);
		print_word(file, counterexample);
		ot.add_counterexample(counterexample);
		file.close();
	}

	iteration++;
	snprintf(filename, 128, "knowledgebase%02d-final.dot", iteration);
	file.open(filename);
	file << knowledge.visualize();
	file.close();

	stats.memory = ot.get_memory_statistics();
	stats.queries.membership = knowledge.count_resolved_queries();

	cout << "required membership queries: " << stats.queries.membership << "\n";
	cout << "required uniq membership queries: " << stats.queries.uniq_membership << "\n";
	cout << "required equivalence queries: " << stats.queries.equivalence << "\n";
	cout << "sizes: bytes: " << stats.memory.bytes
	     << ", members: " << stats.memory.members
	     << ", words: " << stats.memory.words << "\n";
	cout << "upper table rows: " << stats.memory.upper_table
	     << ", lower table rows: " << stats.memory.lower_table
	     << ", columns: " << stats.memory.columns << "\n";
	cout << "original NFA state count: " << nfa->get_state_count() << "\n";
	cout << "minimal DFA state count: " << mindfa_statecount << "\n";
	cout << "final hypothesis state count: " << hypothesis->get_state_count() << "\n";

	delete hypothesis;
	delete nfa;

	if(success)
		return 0;
	else
		return 2;
}

