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
#include <libalf/algorithm_RPNI.h>
#include <libalf/knowledgebase.h>

#include <LanguageGenerator/DFArandomgenerator.h>
#include <LanguageGenerator/NFArandomgenerator.h>
#include <LanguageGenerator/regex_randomgenerator.h>

#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/finite_automaton.h>

#include "amore_alf_glue.h"

using namespace std;
using namespace libalf;
using namespace amore;
using namespace LanguageGenerator;

finite_automaton * angluin_learn_model(logger & log, finite_automaton * model, knowledgebase<bool> & knowledge)
{{{
	angluin_simple_table<bool> angluin(&knowledge, &log, model->get_alphabet_size());

	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	std::set<int> f_initial, f_final;
	multimap<pair<int,int>, int> f_transitions;

	finite_automaton * hypothesis;
	bool equal = false;

	while(!equal) {
		while(! angluin.advance(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions) )
			amore_alf_glue::automaton_answer_knowledgebase(*model, knowledge);

		hypothesis = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!hypothesis) {
			log(LOGGER_ERROR, "angluin: failed to construct hypothesis!\n");
			return NULL;
		}

		list<int> counterexample;
		if(amore_alf_glue::automaton_equivalence_query(*model, *hypothesis, counterexample)) {
			equal = true;
		} else {
			angluin.add_counterexample(counterexample);
			delete hypothesis;
		}
	}

	return hypothesis;
}}}

bool check_validity(logger & log, finite_automaton * model)
{{{
	knowledgebase<bool> knowledge;
	finite_automaton * mdfa;
	RPNI<bool> rpni(&knowledge, &log, model->get_alphabet_size());
	bool ret = true;

	if(NULL == (mdfa = angluin_learn_model(log, model, knowledge))) {
		log(LOGGER_ERROR, "angluin failed!\n");
		return false;
	}

	bool f_is_dfa;
	int f_alphabet_size, f_state_count;
	std::set<int> f_initial, f_final;
	multimap<pair<int,int>, int> f_transitions;

	if(!rpni.advance(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions)) {
		log(LOGGER_ERROR, "rpni failed to advance!\n");
		ret = false;
	} else {
		// compare mdfa and result
		finite_automaton * res;
		res = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(res == NULL) {
			log(LOGGER_ERROR, "construct of rpni failed!\n");
			ret = false;
		} else {
			list<int> cex;
			if(!amore_alf_glue::automaton_equivalence_query(*mdfa, *res, cex)) {
				// dump mdfa and res
				static int bad = 0;
				ofstream file;
				char filename[128];
				snprintf(filename, 128, "model-%02d-mdfa.dot", bad);
				file.open(filename); file << mdfa->generate_dotfile(); file.close();

				snprintf(filename, 128, "model-%02d-rpni.dot", bad);
				file.open(filename); file << res->generate_dotfile(); file.close();

				bad++;
				log(LOGGER_ERROR, "rpni result differs from model! counterexample %s\n", word2string(cex).c_str());
				ret = false;
			}
			delete res;
		}
	}

	delete mdfa;
	return ret;
}}}

int main(int argc, char**argv)
{{{
	ostream_logger log(&cout, LOGGER_WARN);
	int num_testcases, alphabet_size, model_size;

	if(argc != 4) {
		cout << "required parameters: <number of testcases> <alphabet size> <model size>\n";
		return -1;
	}

	num_testcases = atoi(argv[1]);
	alphabet_size = atoi(argv[2]);
	model_size = atoi(argv[3]);

	DFArandomgenerator dfa_rg;
	NFArandomgenerator nfa_rg;
	regex_randomgenerator regex_rg;

	for(int i = 0; i < num_testcases; i++) {
		finite_automaton * model;

		bool f_is_dfa;
		int f_alphabet_size, f_state_count;
		std::set<int> f_initial, f_final;
		multimap<pair<int,int>, int> f_transitions;


		if(!dfa_rg.generate(alphabet_size, model_size, f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		{ log(LOGGER_ERROR, "rg of DFA failed\n"); continue; }
		model = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!model) log(LOGGER_ERROR, "construct of rg DFA failed\n");
		else { if(!check_validity(log, model)) log(LOGGER_ERROR,"random DFA: result different for angluin and rpni!\n"); delete model; }


		if(!nfa_rg.generate(alphabet_size, model_size, 2, 0.5, 0.5, f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		{ log(LOGGER_ERROR, "rg of NFA failed\n"); continue; }
		model = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!model) log(LOGGER_ERROR, "construct of rg DFA failed\n");
		else { if(!check_validity(log, model)) log(LOGGER_ERROR,"random NFA: result different for angluin and rpni!\n"); delete model; }


		std::string regex;
		bool success;
		regex = regex_rg.generate(alphabet_size, model_size, 0.556, 0.278, 0.166);
		model = new nondeterministic_finite_automaton(alphabet_size, regex.c_str(), success);
		if(!success) log(LOGGER_ERROR, "construct of rg RegEx failed\n");
		else { if(!check_validity(log, model)) log(LOGGER_ERROR,"random RegEx: result different for angluin and rpni!\n"); delete model; }

		printf("%6.2f%%\r", (float)i / (float)num_testcases * 100);
		fflush(stdout);

	}
}}}

