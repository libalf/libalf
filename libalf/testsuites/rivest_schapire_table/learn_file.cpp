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
#include <libalf/algorithm_rivest_schapire.h>
#include <libalf/basic_string.h>

#include <libalf/normalizer_msc.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

using namespace std;
using namespace libalf;


int main(int argc, char**argv)
{
	statistics stats;

	amore::finite_automaton *nfa;
	amore::finite_automaton *dfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<bool> knowledge;

	char filename[128];
	ofstream file;
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
		file.open("original-nfa.dot"); file << nfa->visualize(true); file.close();

		dfa = nfa->determinize();
		dfa->minimize();
		mindfa_statecount = dfa->get_state_count();
		file.open("original-dfa.dot"); file << dfa->visualize(true); file.close();
	}}}

	delete nfa;


	// create algorithm and teach it the automaton
	rivest_schapire_table<bool> ot(&knowledge, &log, alphabet_size);
	amore::finite_automaton * hypothesis = NULL;

	for(iteration = 1; iteration <= 100; iteration++) {
		conjecture * cj;

		fflush(stdout);
		printf("advancing...\n");
		while( NULL == (cj = ot.advance()) )
			stats.queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*dfa, knowledge);

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
			basic_string<int32_t>::iterator it;

			snprintf(filename, 128, "table%02d.text.rv", iteration);
			file.open(filename); ot.print(file); file.close();

			/*
			serialized = ot.serialize();

			snprintf(filename, 128, "table%02d.serialized.rv", iteration);
			file.open(filename);

			for(it = serialized.begin(); it != serialized.end(); it++) {
				file << ntohl(*it);
				file << ";";
			}

			file.close();
			*/
		}}}

#ifdef DEBUG_RIVEST_SCHAPIRE
		cout << ot.to_string();
#endif

		snprintf(filename, 128, "hypothesis%02d.dot", iteration);
		file.open(filename); file << hypothesis->visualize(true); file.close();

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		list<int> counterexample;

		stats.queries.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*dfa, *hypothesis, counterexample)) {
			// equivalent
			cout << "success.\n";
			success = true;
			break;
		}

		snprintf(filename, 128, "counterexample%02d.rv", iteration);
		file.open(filename);
		print_word(file, counterexample);
		ot.add_counterexample(counterexample);
		file.close();
	}

	iteration++;

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
	cout << "original NFA state count: " << dfa->get_state_count() << "\n";
	cout << "minimal DFA state count: " << mindfa_statecount << "\n";
	cout << "final hypothesis state count: " << hypothesis->get_state_count() << "\n";

	delete hypothesis;
	delete dfa;

	if(success)
		return 0;
	else
		return 2;
}

