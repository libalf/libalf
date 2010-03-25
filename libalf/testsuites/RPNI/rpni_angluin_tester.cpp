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
#include <libalf/algorithm_angluin.h>
#include <libalf/algorithm_RPNI.h>
#include <libalf/knowledgebase.h>

#include <liblangen/dfa_randomgenerator.h>
#include <liblangen/nfa_randomgenerator.h>
#include <liblangen/regex_randomgenerator.h>

#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>
#include <amore++/finite_automaton.h>

#include "amore_alf_glue.h"

using namespace std;
using namespace libalf;
using namespace amore;
using namespace liblangen;

finite_automaton * angluin_learn_model(logger & log, finite_automaton * model, knowledgebase<bool> & knowledge)
{{{
	angluin_simple_table<bool> angluin(&knowledge, &log, model->get_alphabet_size());

	finite_automaton * hypothesis;
	bool equal = false;

	while(!equal) {
		conjecture *cj;
		simple_moore_machine *ba;
		while( NULL == (cj = angluin.advance()) )
			amore_alf_glue::automaton_answer_knowledgebase(*model, knowledge);

		ba = dynamic_cast<simple_moore_machine*>(cj);
		hypothesis = construct_amore_automaton(ba->is_deterministic, ba->input_alphabet_size, ba->state_count, ba->initial_states, ba->final_states, ba->transitions);
		delete cj;
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

	conjecture *cj;

	if(!rpni.conjecture_ready()) {
		log(LOGGER_WARN, "RPNI says that no conjecture is ready! trying anyway...\n");
	}

	if(NULL == (cj = rpni.advance()) ) {
		log(LOGGER_ERROR, "RPNI failed to advance!\n");
		ret = false;
	} else {
		// compare mdfa and result
		simple_moore_machine *ba;
		ba = dynamic_cast<simple_moore_machine*>(cj);
		finite_automaton * res;
		res = construct_amore_automaton(ba->is_deterministic, ba->input_alphabet_size, ba->state_count, ba->initial_states, ba->final_states, ba->transitions);
		delete cj;
		if(res == NULL) {
			log(LOGGER_ERROR, "construct of RPNI failed!\n");
			ret = false;
		} else {
			list<int> cex;
			if(!amore_alf_glue::automaton_equivalence_query(*mdfa, *res, cex))
			{{{ // dump mdfa, res and knowledgebase
				static int bad = 0;
				ofstream file;
				char filename[128];
				basic_string<int32_t> serialized;

				snprintf(filename, 128, "model-%02d-mdfa.dot", bad);
				file.open(filename); file << mdfa->visualize(); file.close();

				snprintf(filename, 128, "model-%02d-mdfa.bs", bad);
				serialized = mdfa->serialize();
				basic_string_to_file(serialized, filename);

				snprintf(filename, 128, "model-%02d-rpni.dot", bad);
				file.open(filename); file << res->visualize(); file.close();

				snprintf(filename, 128, "model-%02d-rpni.bs", bad);
				serialized = res->serialize();
				basic_string_to_file(serialized, filename);

				snprintf(filename, 128, "model-%02d-base.bs", bad);
				serialized = knowledge.serialize();
				basic_string_to_file(serialized, filename);

				snprintf(filename, 128, "model-%02d-base.txt", bad);
				file.open(filename); file << knowledge.to_string(); file.close();

				bad++;
				log(LOGGER_ERROR, "RPNI result differs from model! counterexample %s\n", word2string(cex).c_str());
				ret = false;
			}}}
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

	dfa_randomgenerator dfa_rg;
	nfa_randomgenerator nfa_rg;
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
		else { if(!check_validity(log, model)) log(LOGGER_ERROR,"random DFA: result different for Angluin and RPNI!\n"); delete model; }

		cout << "#\n";

		if(!nfa_rg.generate(alphabet_size, model_size, 2, 0.5, 0.5, f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions))
		{ log(LOGGER_ERROR, "rg of NFA failed\n"); continue; }
		model = construct_amore_automaton(f_is_dfa, f_alphabet_size, f_state_count, f_initial, f_final, f_transitions);
		if(!model) log(LOGGER_ERROR, "construct of rg DFA failed\n");
		else { if(!check_validity(log, model)) log(LOGGER_ERROR,"random NFA: result different for Angluin and RPNI!\n"); delete model; }

		cout << "#\n";

		std::string regex;
		bool success;
		regex = regex_rg.generate(alphabet_size, model_size, 0.556, 0.278, 0.166);
		model = new nondeterministic_finite_automaton(alphabet_size, regex.c_str(), success);
		if(!success) log(LOGGER_ERROR, "construct of rg RegEx failed\n");
		else { if(!check_validity(log, model)) log(LOGGER_ERROR,"random RegEx: result different for Angluin and RPNI!\n"); delete model; }

		cout << "#\n";

		printf("%6.2f%%\r", (float)i / (float)num_testcases * 100);
		fflush(stdout);

	}
}}}

