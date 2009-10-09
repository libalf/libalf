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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

// Performance tester for online algorithms

#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <algorithm>

#include <liblangen/dfa_randomgenerator.h>

#include <amore++/finite_automaton.h>
#include <amore++/nondeterministic_finite_automaton.h>
#include <amore++/deterministic_finite_automaton.h>

#include <libalf/alf.h>
#include <libalf/statistics.h>

#include <libalf/algorithm_angluin.h>
#include <libalf/algorithm_NLstar.h>

//////
#define ALGORITHM angluin_simple_table
#define ALGORITHM_NAME "angluin_simple_table"
//////

#include "amore_alf_glue.h"

using namespace std;
using namespace liblangen;
using namespace amore;
using namespace libalf;

ostream_logger *log;

bool learn(finite_automaton * model, unsigned int & membership_queries, unsigned int & eq_queries, unsigned long long int & memory_usage, unsigned long long int & usecs_in_alg, unsigned long long int & usecs_total)
{{{
	membership_queries = 0;
	eq_queries = 0;
	memory_usage = 0;
	usecs_in_alg = 0;
	usecs_total = 0;

	// total time
	struct rusage ru;

	struct timeval start_total_time;
	getrusage(RUSAGE_THREAD /* or RUSAGE_SELF, if not defined */, &ru);
	start_total_time = ru.ru_utime;

	knowledgebase<bool> kb;
	ALGORITHM<bool> alg(&kb, log, model->get_alphabet_size());

	alg.enable_timing();

	for(unsigned int iteration = 0; iteration < model->get_state_count(); iteration++) {
		conjecture * cj;
		simple_automaton * sa;
		list<int> counterexample;

		while( NULL == (cj = alg.advance()) )
			membership_queries += amore_alf_glue::automaton_answer_knowledgebase(*model, kb);

		sa = dynamic_cast<simple_automaton*>(cj);

		eq_queries++;
		if(amore_alf_glue::automaton_equivalence_query(*model, cj, counterexample)) {
			delete cj;
			break;
		}
		delete cj;

		alg.add_counterexample(counterexample);
	}

	// total time
	getrusage(RUSAGE_THREAD /* or RUSAGE_SELF, if not defined */, &ru);
	struct timeval total;
	timersub(&(ru.ru_utime), &start_total_time, &total);
	usecs_total += total.tv_sec * 1000000;
	usecs_total += total.tv_usec;

	// algorithm stats
	memory_statistics mstat;
	mstat = alg.get_memory_statistics();
	memory_usage += mstat.bytes;
	memory_usage += kb.get_memory_usage();

	timing_statistics tstat;
	tstat = alg.get_timing_statistics();
	usecs_in_alg += tstat.user_sec * 1000000;
	usecs_in_alg += tstat.user_usec;

	return true;
}}}

int main(int argc, char**argv)
{{{
	if(argc != 6) {
		cout << "please give the following parameters:\n"
			"\t1: number of testcases per asize/msize\n"
			"\t2: alphabet size\n"
			"\t3: minimal model size\n"
			"\t4: maximal model size\n"
			"\t5: steps model size\n";

		return 1;
	};

	unsigned int num_testcases    = atoi(argv[1]);
	unsigned int alphabet_size    = atoi(argv[2]);
	unsigned int min_model_size   = atoi(argv[3]);
	unsigned int max_model_size   = atoi(argv[4]);
	unsigned int steps_model_size = atoi(argv[5]);

	log = new ostream_logger(&cout, LOGGER_DEBUG);
	dfa_randomgenerator dfarg;

	(*log)(LOGGER_INFO, "%d automata, asize %d, model size %d..%d += %d\n",
			num_testcases, alphabet_size, min_model_size, max_model_size, steps_model_size);

	char str[512];


	ofstream single_stats;
	ofstream average_stats;

	snprintf(str, 512, "stats-%s/num%dasize%dmsize%d..%dstep%d.stat", ALGORITHM_NAME, num_testcases, alphabet_size, min_model_size, max_model_size, steps_model_size);
	single_stats.open(str);
	snprintf(str, 512, "stats-%s/num%dasize%dmsize%d..%dstep%d-avg.stat", ALGORITHM_NAME, num_testcases, alphabet_size, min_model_size, max_model_size, steps_model_size);
	average_stats.open(str);

	// learning loop
	simple_automaton sa;
	for(unsigned int model_size = min_model_size; model_size <= max_model_size; model_size += steps_model_size) {
		unsigned int avg_membership_queries = 0, avg_eq_queries = 0;
		unsigned long long int avg_memory_usage = 0, avg_usecs_in_alg = 0, avg_usecs_total = 0;
		for(unsigned int c = 0; c < num_testcases; c++) {
			sa.clear();
			finite_automaton * model;
		create_model:
			if(!dfarg.generate(alphabet_size, model_size, sa.is_deterministic, sa.alphabet_size, sa.state_count, sa.initial, sa.final, sa.transitions)) {
				(*log)(LOGGER_WARN, "DFA random generator failed to construct model (asize %d msize %d)\n", alphabet_size, model_size);
				goto create_model;
			}
			model = construct_amore_automaton(sa.is_deterministic, sa.alphabet_size, sa.state_count, sa.initial, sa.final, sa.transitions);
			if(!model) {
				(*log)(LOGGER_WARN, "failed to construct amore automaton for model\n");
				goto create_model;
			}
			model->minimize();
			if(model->get_state_count() != model_size) {
				(*log)(LOGGER_DEBUG, "model skipped: too small.\n");
				delete model;
				goto create_model;
			}

			unsigned int membership_queries, eq_queries;
			unsigned long long int memory_usage, usecs_in_alg, usecs_total;

			if(!learn(model, membership_queries, eq_queries, memory_usage, usecs_in_alg, usecs_total)) {
				string s;
				s = sa.write();
				(*log)(LOGGER_ERROR, "failed to learn model! model:\n\n%s\n", s.c_str());
				delete model;
				return -1;
			}

			snprintf(str, 512, "%s  aSize %02d mSize %04d   mQueries %06d eQueries %03d  mUsage %08llu  usecsAlg %09llu usecsT %09llu\n",
					ALGORITHM_NAME, alphabet_size, model_size,
					membership_queries, eq_queries, memory_usage, usecs_in_alg, usecs_total);

			(*log)(LOGGER_DEBUG, "%s", str);
			single_stats << str;
			single_stats.flush();

			avg_membership_queries += membership_queries;
			avg_eq_queries += eq_queries;
			avg_memory_usage += memory_usage;
			avg_usecs_in_alg += usecs_in_alg;
			avg_usecs_total += usecs_total;

			delete model;
		}
		avg_membership_queries /= num_testcases;
		avg_eq_queries /= num_testcases;
		avg_memory_usage /= num_testcases;
		avg_usecs_in_alg /= num_testcases;
		avg_usecs_total /= num_testcases;

		snprintf(str, 512, "%s-AVERAGED  aSize %02d mSize %04d   mQueries %06d eQueries %03d  mUsage %08llu  usecsAlg %09llu usecsT %09llu\n",
				ALGORITHM_NAME, alphabet_size, model_size,
				avg_membership_queries, avg_eq_queries, avg_memory_usage, avg_usecs_in_alg, avg_usecs_total);

		(*log)(LOGGER_INFO, "%s", str);
		average_stats << str;
		average_stats.flush();
	}

	single_stats.close();
	average_stats.close();

	delete log;
	dfarg.discard_tables();

	return 0;
}}}

