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
#include <amore++/deterministic_finite_automaton.h>

#include <liblangen/dfa_randomgenerator.h>

#include "amore_alf_glue.h"

using namespace std;
using namespace libalf;
using namespace liblangen;

ostream_logger my_logger(&cout, LOGGER_DEBUG);

int learn_via_NLstar(int asize, amore::finite_automaton * model)
{{{
	statistics stats;
	knowledgebase<bool> knowledge;

	int iteration;
	bool success = false;

	// create NLstar table and teach it the automaton
	NLstar_table<bool> ot(&knowledge, &my_logger, asize);

	for(iteration = 1; iteration <= 100; iteration++) {
		conjecture *cj;

		while( NULL == (cj = ot.advance()) ) {
			// resolve missing knowledge:
			stats.queries.uniq_membership += amore_alf_glue::automaton_answer_knowledgebase(*model, knowledge);
		}


		list<int> counterexample;
		stats.queries.equivalence++;
		if(amore_alf_glue::automaton_equivalence_query(*model, cj, counterexample)) {
			// equivalent
			cout << "success.\n";
			success = true;
			break;
		}
		delete cj;

		ot.add_counterexample(counterexample);
	}

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
	map<int, map<int, set<int> > > f_transitions;

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

	dfa_randomgenerator drng;

	while(1) {
		drng.generate(asize, size,  f_is_dfa, f_asize, f_state_count, f_initial, f_final, f_transitions);
		amore::finite_automaton *model = amore::construct_amore_automaton(f_is_dfa, f_asize, f_state_count, f_initial, f_final, f_transitions);
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
				file.open(filename); file << model->visualize(); file.close();
				snprintf(filename, 128, "hit-a%d-s%d-%02d.atm", asize, size, num);
				basic_string_to_file(serialized, filename);
				my_logger(LOGGER_WARN, "\nmatch found with asize %d, state count %d, eq queries %d. saved as %s.\n",
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

