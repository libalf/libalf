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

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

using namespace std;
using namespace libalf;

int main(int argc, char**argv)
{
	statistics stats;

	amore::finite_automaton *nfa, *dfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<bool> knowledge;

	char filename[128];
	ofstream file;

	int iteration = 0;
	bool success = false;

	int alphabet_size;
	unsigned int hypothesis_state_count = 0;

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
		file.open("original-nfa.dot"); file << nfa->visualize(); file.close();

		dfa = nfa->determinize();
		dfa->minimize();
		file.open("original-dfa.dot"); file << dfa->visualize(); file.close();

		basic_string<int32_t> serial;
		serial = dfa->serialize();
		libalf::basic_string_to_file(serial, "original-dfa.ser");
	}}}

	delete nfa;

	// create kearns/vazirani learning algorithm and teach it the automaton
	kearns_vazirani<bool> ot(&knowledge, &log, alphabet_size, true);
	amore::finite_automaton * hypothesis = NULL;

	while(!success) {
		iteration++;
		int c = 'a';
		conjecture * cj;

		while( NULL == (cj = ot.advance()) ) {
			// resolve missing knowledge:

			//snprintf(filename, 128, "knowledgebase%02d%c.dot", iteration, c);
			//file.open(filename); file << knowledge.visualize(); file.close();

			// create query-tree
			knowledgebase<bool> * query;
			query = knowledge.create_query_tree();

			//snprintf(filename, 128, "knowledgebase%02d%c-q.dot", iteration, c);
			//file.open(filename); file << query->visualize(); file.close();

			// answer queries
			stats.queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*dfa, *query);

			//snprintf(filename, 128, "knowledgebase%02d%c-r.dot", iteration, c);
			//file.open(filename); file << query->visualize(); file.close();

			// merge answers into knowledgebase
			knowledge.merge_knowledgebase(*query);
			delete query;
			c++;
		}

		libalf::finite_automaton * ba = dynamic_cast<libalf::finite_automaton*>(cj);

		// DEBUG
#if 0
		cout << ot.to_string() << endl;
		cout << ba->visualize() << endl;
#endif

		// END DEBUG

		ot.to_string();

		if(hypothesis)
			delete hypothesis;
		hypothesis = amore_alf_glue::automaton_libalf2amore(*ba);
		delete cj;

		if(!hypothesis) {
			printf("generation of hypothesis failed!\n");
			return -1;
		}

		{{{ /* dot the tree */

			//snprintf(filename, 128, "tree%02d.dot", iteration);
			//file.open(filename); ot.print(file); file.close();

		}}}

		//snprintf(filename, 128, "hypothesis%02d.dot", iteration);
		//file.open(filename); file << hypothesis->visualize(); file.close();

		printf("hypothesis %02d state count %02d\n", iteration, hypothesis->get_state_count());
		//if(hypothesis_state_count >= hypothesis->get_state_count()) {
		//	log(LOGGER_ERROR, "STATE COUNT DID NOT INCREASE\n");
		//}
		hypothesis_state_count = hypothesis->get_state_count();
		
		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		list<int> counterexample;
		stats.queries.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*dfa, *hypothesis, counterexample)) {
			// equivalent
			success = true;
			
			// Print learned automata
			snprintf(filename, 128, "result.dot");
			file.open(filename); file << hypothesis->visualize(); file.close();
						
			break;
		}

		//snprintf(filename, 128, "counterexample%02d.KVtree", iteration);
		//file.open(filename); print_word(file, counterexample); file.close();
		ot.add_counterexample(counterexample);
		
	}

	snprintf(filename, 128, "tree.dot");
	file.open(filename); ot.print(file); file.close();
	
	
	
	iteration++;
	snprintf(filename, 128, "knowledgebase%02d-final.dot", iteration);
	file.open(filename);
	file << knowledge.visualize();
	file.close();

	stats.memory = ot.get_memory_statistics();
	stats.queries.membership = knowledge.count_resolved_queries();

	delete dfa;

	cout << "\nrequired membership queries: " << stats.queries.membership << "\n";
	cout << "required uniq membership queries: " << stats.queries.uniq_membership << "\n";
	cout << "required equivalence queries: " << stats.queries.equivalence << "\n";
	cout << "sizes: bytes: " << stats.memory.bytes
	     << ", members: " << stats.memory.members
	     << ", words: " << stats.memory.words << "\n";
	cout << "minimal state count: " << hypothesis->get_state_count() << "\n";

	delete hypothesis;
	
	if(success)
		return 0;
	else
		return 2;
}

