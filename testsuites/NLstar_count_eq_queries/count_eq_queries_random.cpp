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
	knowledge.set_statistics(&stats);

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
			stats.query_count.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*model, knowledge);

		if(hypothesis)
			delete hypothesis;
		hypothesis = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!hypothesis) {
			cout << "generation of hypothesis failed!\n";
			return -1;
		}

		list<int> counterexample;
		stats.query_count.equivalence++;
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
		return stats.query_count.equivalence;
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

	int asize = 3;
	int size = 6;
	DFArandomgenerator drng;

	while(1) {
		drng.generate(asize, size,  f_is_dfa, f_asize, f_state_count, f_initial, f_final, f_transitions);
		finite_automaton *model = construct_amore_automaton(f_is_dfa, f_asize, f_state_count, f_initial, f_final, f_transitions);
		model->minimize();

		if(model->get_state_count() < size) {
			cout << ".";
			delete model;
			continue;
		}
		cout << "+";

		int eq_queries = learn_via_NLstar(asize, model);

		if(eq_queries >  size) {
			char filename[128];
			ofstream file;
			snprintf(filename, 128, "hit-%02d.dot", num);
			basic_string<int32_t> serialized = model->serialize();
			file.open(filename); file << model->generate_dotfile(); file.close();
			snprintf(filename, 128, "hit-%02d.atm", num);
			basic_string_to_file(serialized, filename);
			log(LOGGER_WARN, "\nmatch found with asize %d, state count %d, eq queries %d. saved as %s.\n",
					asize, size, eq_queries, filename);
			num++;
		}
		delete model;
	}
}}}

