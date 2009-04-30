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

#include <libalf/alf.h>
#include <libalf/algorithm_angluin.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;
using namespace amore;

nondeterministic_finite_automaton * sample_automaton()
{{{
	nondeterministic_finite_automaton * nfa;

	/*
	int automaton[] = {
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
//	int automaton[] = { 2, 4, 1, 0, 4, 0,1,2,3, 7, 0,0,1, 0,1,1, 1,0,2, 1,1,2, 2,0,3, 2,1,3, 3,1,0 };

	// Fig. 13: (NFA)
//	int automaton[] = { 2, 6, 1, 0, 5, 0,1,2,3,4, 12, 0,1,1, 0,0,2, 1,1,1, 1,0,3, 2,0,2, 2,1,4, 3,0,5, 3,1,5, 4,1,4, 4,0,5, 5,0,5, 5,1,5 };

	// Fig. 15: (NFA)
//	int automaton[] = { 3, 7, 1, 0, 4, 0,1,4,5, 21, 0,0,1, 0,1,2, 0,2,3, 1,0,1, 1,1,4, 1,2,2, 2,0,5, 2,1,4, 2,2,4, 3,0,5, 3,1,3, 3,2,5, 4,0,1, 4,1,4, 4,2,4, 5,0,6, 5,1,3, 5,2,3, 6,0,5, 6,1,1, 6,2,2 };

	// Fig. 16: (NFA)
//	int automaton[] = { 2, 4, 1, 0, 2, 1,2, 6, 0,1,1, 1,0,2, 1,1,1, 2,1,3, 3,0,2, 3,1,2, };

	// Fig. 18: (NFA)
//	int automaton[] = { 2, 10, 1, 0, 5, 0,1,2,8,9, 12, 0,0,1, 0,1,2, 1,0,3, 1,1,7, 2,1,2, 3,0,4, 4,0,3, 4,1,5, 5,1,6, 6,1,9, 7,1,8, 8,1,9 };

	// Fig. 24: (NFA)
	int automaton[] = { 2, 6, 1, 0, 2, 3,5, 6, 0,1,1, 1,0,2, 2,0,3, 3,0,5, 3,1,4, 4,0,5 };

	basic_string<int32_t> serial;
	basic_string<int32_t>::iterator si;

	serial += htonl(sizeof(automaton)/sizeof(int));
	for(unsigned int i = 0; i < sizeof(automaton)/sizeof(int); i++) {
		serial += htonl(automaton[i]);
	}

	si = serial.begin();
	nfa = new nondeterministic_finite_automaton;
	nfa->deserialize(si, serial.end());

	return nfa;
}}}

int main(int argc, char**argv)
{
	statistics stats;

	finite_automaton *nfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<ANSWERTYPE> knowledge;

	char filename[128];
	ofstream file;

	int iteration;
	bool success = false;

	int alphabet_size;
	int hypothesis_state_count = 0;

	nfa = sample_automaton();

	alphabet_size = nfa->get_alphabet_size();

	{{{ /* dump original automata */
		file.open("original-nfa.dot"); file << nfa->generate_dotfile(); file.close();

		finite_automaton * dfa;
		dfa = nfa->determinize();
		dfa->minimize();
		file.open("original-dfa.dot"); file << dfa->generate_dotfile(); file.close();
		delete dfa;
	}}}


	// create oracle instance and teacher instance
	knowledge.set_statistics(&stats);

	// create angluin_simple_table and teach it the automaton
	angluin_simple_table<ANSWERTYPE> ot(&knowledge, &log, alphabet_size);
	finite_automaton * hypothesis = NULL;

	for(iteration = 1; iteration <= 100; iteration++) {
		int c = 'a';
		bool f_is_dfa;
		int f_alphabet_size, f_state_count;
		set<int> f_initial, f_final;
		multimap<pair<int, int>, int> f_transitions;

		while( ! ot.advance(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions) ) {
			// resolve missing knowledge:

			snprintf(filename, 128, "knowledgebase%02d%c.dot", iteration, c);
			file.open(filename); file << knowledge.generate_dotfile(); file.close();

			// create query-tree
			knowledgebase<ANSWERTYPE> * query;
			query = knowledge.create_query_tree();

			snprintf(filename, 128, "knowledgebase%02d%c-q.dot", iteration, c);
			file.open(filename); file << query->generate_dotfile(); file.close();

			// answer queries
			stats.query_count.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*nfa, *query);

			snprintf(filename, 128, "knowledgebase%02d%c-r.dot", iteration, c);
			file.open(filename); file << query->generate_dotfile(); file.close();

			// merge answers into knowledgebase
			knowledge.merge_knowledgebase(*query);
			delete query;
			c++;
		}

		if(hypothesis)
			delete hypothesis;
		hypothesis = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!hypothesis) {
			printf("generation of hypothesis failed!\n");
			return -1;
		}

		{{{ /* dump/serialize table */
			basic_string<int32_t> serialized;
			basic_string<int32_t>::iterator it;

			snprintf(filename, 128, "table%02d.text.angluin", iteration);
			file.open(filename); ot.print(file); file.close();

			/*
			serialized = ot.serialize();

			snprintf(filename, 128, "table%02d.serialized.angluin", iteration);
			file.open(filename);

			for(it = serialized.begin(); it != serialized.end(); it++) {
				file << ntohl(*it);
				file << ";";
			}

			file.close();
			*/
		}}}

		snprintf(filename, 128, "hypothesis%02d.dot", iteration);
		file.open(filename); file << hypothesis->generate_dotfile(); file.close();

		printf("hypothesis %02d state count %02d\n", iteration, hypothesis->get_state_count());
		if(hypothesis_state_count >= hypothesis->get_state_count()) {
			log(LOGGER_ERROR, "STATE COUNT DID NOT INCREASE\n");
			getchar();
		}
		hypothesis_state_count = hypothesis->get_state_count();

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		list<int> counterexample;
		stats.query_count.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*nfa, *hypothesis, counterexample)) {
			// equivalent
			cout << "success.\n";
			success = true;
			break;
		}

		snprintf(filename, 128, "counterexample%02d.angluin", iteration);
		file.open(filename);
		print_word(file, counterexample);
		ot.add_counterexample(counterexample);
		file.close();
	}

	iteration++;
	snprintf(filename, 128, "knowledgebase%02d-final.dot", iteration);
	file.open(filename);
	file << knowledge.generate_dotfile();
	file.close();

	ot.get_memory_statistics(stats);

	delete nfa;

	cout << "\nrequired membership queries: " << stats.query_count.membership << "\n";
	cout << "required uniq membership queries: " << stats.query_count.uniq_membership << "\n";
	cout << "required equivalence queries: " << stats.query_count.equivalence << "\n";
	cout << "sizes: bytes: " << stats.table_size.bytes
	     << ", members: " << stats.table_size.members
	     << ", words: " << stats.table_size.words << "\n";
	cout << "upper table rows: " << stats.table_size.upper_table
	     << ", lower table rows: " << stats.table_size.lower_table
	     << ", columns: " << stats.table_size.columns << "\n";
	cout << "minimal state count: " << hypothesis->get_state_count() << "\n";

	delete hypothesis;

	if(success)
		return 0;
	else
		return 2;
}

