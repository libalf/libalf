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

#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>

#include <LanguageGenerator/DFArandomgenerator.h>

#include "amore_alf_glue.h"

//#define ANSWERTYPE extended_bool
#define ANSWERTYPE bool

using namespace std;
using namespace libalf;
using namespace amore;
using namespace LanguageGenerator;

ostream_logger log(&cout, LOGGER_DEBUG);

int learn_via_NLstar(int asize, finite_automaton * model)
{{{
	statistics stats;
	knowledgebase<ANSWERTYPE> knowledge;

	int iteration;
	bool success = false;

	// create NLstar table and teach it the automaton
	NLstar_table<ANSWERTYPE> ot(&knowledge, &log, asize);
	finite_automaton * hypothesis = NULL;

	for(iteration = 1; iteration <= 100; iteration++) {
		bool f_is_dfa;
		int f_alphabet_size, f_state_count;
		set<int> f_initial, f_final;
		multimap<pair<int, int>, int> f_transitions;

		while( ! ot.advance(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions) )
			stats.queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*model, knowledge);

		if(hypothesis)
			delete hypothesis;
		hypothesis = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!hypothesis) {
			cout << "generation of hypothesis failed!\n";
			return -1;
		}

		list<int> counterexample;
		stats.queries.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*model, *hypothesis, counterexample)) {
			// equivalent
			success = true;
			break;
		}

		ot.add_counterexample(counterexample);
	}

	if(hypothesis)
		delete hypothesis;

	if(success) {
//		cout << "success.\n";
		return stats.queries.equivalence;
	} else {
		cout << "failed!\n";
		return -1;
	}
}}}

int main(int argc, char**argv)
{{{
	bool f_is_dfa;
	int f_asize, f_state_count;
	set<int> f_initial, f_final;
	multimap<pair<int,int>, int> f_transitions;

	int num = 0;

	if(argc != 3) {
		printf("give asize and state-count as parameter\n");
		return -1;
	}
	int asize = atoi(argv[1]);
	unsigned int size = atoi(argv[2]);

	int checked = 0;
	int skipped = 0;
	int found = 0;

	int print_skipper = 0;

	DFArandomgenerator drng;

	while(1) {
		drng.generate(asize, size,  f_is_dfa, f_asize, f_state_count, f_initial, f_final, f_transitions);
		finite_automaton *model = construct_amore_automaton(f_is_dfa, f_asize, f_state_count, f_initial, f_final, f_transitions);
		model->minimize();

		if(model->get_state_count() < size) {
			skipped++;
		} else {
			checked++;

			unsigned int eq_queries = learn_via_NLstar(asize, model);

			if(eq_queries >  size) {
				found++;
				char filename[128];
				ofstream file;
				snprintf(filename, 128, "hit-a%d-s%d-%02d.dot", asize, size, num);
				basic_string<int32_t> serialized = model->serialize();
				file.open(filename); file << model->generate_dotfile(); file.close();
				snprintf(filename, 128, "hit-a%d-s%d-%02d.atm", asize, size, num);
				basic_string_to_file(serialized, filename);
				log(LOGGER_WARN, "\nmatch found with asize %d, state count %d, eq queries %d. saved as %s.\n",
						asize, size, eq_queries, filename);
				num++;
			}
		}
		delete model;

		if(checked > 0) {
			print_skipper++;
			print_skipper %= 10;
			if(print_skipper == 0) {
				printf("asize %d, states %d; %d, checked %d, found %d (%f%% of checked)    \r",
					asize, size,
					skipped+checked, checked, found, ((double)found) / checked * 100);
			}
		}
	}
}}}

