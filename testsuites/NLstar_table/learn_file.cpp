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
#include <libalf/algorithm_NLstar.h>
#include <libalf/basic_string.h>

#include <amore++/nondeterministic_finite_automaton.h>

#include "amore_alf_glue.h"

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;
using namespace amore;

int main(int argc, char**argv)
{
	statistics stats;

	finite_automaton *nfa;
	ostream_logger log(&cout, LOGGER_DEBUG);

	knowledgebase<ANSWERTYPE> knowledge;

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
		basic_string<int32_t>::iterator si;
		if(!file_to_basic_string(argv[1], str)) {
			cout << "failed to load file \"" << argv[1] << "\".\n";
			return -1;
		}
		nfa = new nondeterministic_finite_automaton;
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
		file.open("original-nfa.dot"); file << nfa->generate_dotfile(); file.close();

		finite_automaton * dfa;
		dfa = nfa->determinize();
		dfa->minimize();
		mindfa_statecount = dfa->get_state_count();
		file.open("original-dfa.dot"); file << dfa->generate_dotfile(); file.close();
		delete dfa;
	}}}


	// create NLstar table and teach it the automaton
	NLstar_table<ANSWERTYPE> ot(&knowledge, &log, alphabet_size);
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
		file.open(filename); file << hypothesis->generate_dotfile(); file.close();

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

		snprintf(filename, 128, "counterexample%02d.NLstar", iteration);
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
	stats.query_count.membership = knowledge.count_resolved_queries();

	cout << "required membership queries: " << stats.query_count.membership << "\n";
	cout << "required uniq membership queries: " << stats.query_count.uniq_membership << "\n";
	cout << "required equivalence queries: " << stats.query_count.equivalence << "\n";
	cout << "sizes: bytes: " << stats.table_size.bytes
	     << ", members: " << stats.table_size.members
	     << ", words: " << stats.table_size.words << "\n";
	cout << "upper table rows: " << stats.table_size.upper_table
	     << ", lower table rows: " << stats.table_size.lower_table
	     << ", columns: " << stats.table_size.columns << "\n";
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

