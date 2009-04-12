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

	bool regex_ok;
	if(argc == 3) {
		nfa = new nondeterministic_finite_automaton(atoi(argv[1]), argv[2], regex_ok);
	} else /* find alphabet size or show some example regex */ {{{
		if(argc == 2) {
			nfa = new nondeterministic_finite_automaton(argv[1], regex_ok);
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
	}}}

	if(regex_ok) {
		log(LOGGER_INFO, "REGEX ok.\n");
	} else {
		log(LOGGER_ERROR, "REGEX failed.\n");
		return 1;
	}

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

	// create angluin_simple_observationtable and teach it the automaton
	angluin_simple_observationtable<ANSWERTYPE> ot(&knowledge, &log, alphabet_size);
	amore_alf_glue::amore_automaton_holder hypothesis;

	for(iteration = 1; iteration <= 100; iteration++) {
		int c = 'a';
		while( ! ot.advance(&hypothesis) ) {
			// resolve missing knowledge:

			snprintf(filename, 128, "knowledgebase%2d%c.dot", iteration, c);
			file.open(filename); file << knowledge.generate_dotfile(); file.close();

			// create query-tree
			knowledgebase<ANSWERTYPE> * query;
			query = knowledge.create_query_tree();

			snprintf(filename, 128, "knowledgebase%2d%c-q.dot", iteration, c);
			file.open(filename); file << query->generate_dotfile(); file.close();

			// answer queries
			stats.query_count.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*nfa, *query);

			snprintf(filename, 128, "knowledgebase%2d%c-r.dot", iteration, c);
			file.open(filename); file << query->generate_dotfile(); file.close();

			// merge answers into knowledgebase
			knowledge.merge_knowledgebase(*query);
			delete query;
			c++;
		}


		{{{ /* dump/serialize observationtable */
			basic_string<int32_t> serialized;
			basic_string<int32_t>::iterator it;

			snprintf(filename, 128, "observationtable%2d.text.angluin", iteration);
			file.open(filename); ot.print(file); file.close();

			serialized = ot.serialize();

			snprintf(filename, 128, "observationtable%2d.serialized.angluin", iteration);
			file.open(filename);

			for(it = serialized.begin(); it != serialized.end(); it++) {
				file << ntohl(*it);
				file << ";";
			}

			file.close();
		}}}

		snprintf(filename, 128, "hypothesis%2d.dot", iteration);
		file.open(filename); file << hypothesis.get_automaton()->generate_dotfile(); file.close();

		// once an automaton is generated, test for equivalence with oracle_automaton
		// if this test is ok, all worked well

		list<int> counterexample;
		if(amore_alf_glue::automaton_equivalence_query(*nfa, *(hypothesis.get_automaton()), counterexample)) {
			// equivalent
			cout << "success.\n";
			success = true;
			break;
		}

		snprintf(filename, 128, "counterexample%2d.angluin", iteration);
		file.open(filename);
		print_word(file, counterexample);
		ot.add_counterexample(counterexample);
		file.close();
	}

	iteration++;
	snprintf(filename, 128, "knowledgebase%2d-final.dot", iteration);
	file.open(filename);
	file << knowledge.generate_dotfile();
	file.close();

	ot.get_memory_statistics(stats);

	delete nfa;

	cout << "required membership queries: " << stats.query_count.membership << "\n";
	cout << "required uniq membership queries: " << stats.query_count.uniq_membership << "\n";
	cout << "required equivalence queries: " << stats.query_count.equivalence << "\n";
	cout << "sizes: bytes: " << stats.table_size.bytes
	     << ", members: " << stats.table_size.members
	     << ", words: " << stats.table_size.words << "\n";
	cout << "upper table rows: " << stats.table_size.upper_table
	     << ", lower table rows: " << stats.table_size.lower_table
	     << ", columns: " << stats.table_size.columns << "\n";
	cout << "minimal state count: " << hypothesis.get_automaton()->get_state_count() << "\n";

	if(success)
		return 0;
	else
		return 2;
}

