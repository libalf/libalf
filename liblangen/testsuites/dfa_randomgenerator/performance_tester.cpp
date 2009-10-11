/* $Id$
 * vim: fdm=marker
 *
 * This file is part of liblangen (LANguageGENerator)
 *
 * liblangen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblangen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with liblangen.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

/*
 * performance_tester: speed tests for DFA random generator.
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include <liblangen/dfa_randomgenerator.h>

using namespace liblangen;
using namespace std;

long double exact_time()
{{{
	struct timeval tmp;

	gettimeofday(&tmp, NULL);
	long double result = tmp.tv_sec + ((long double) tmp.tv_usec) / 1000000.0;

	return result;
}}}

long double generate(int alphabet_size, int state_count, int count)
{{{
	long double time;
	dfa_randomgenerator rag;

	time = exact_time();
	while(count > 0) {
		int asize, scount;
		bool is_dfa;
		set<int> ini, fini;
		multimap<pair<int,int>, int> tra;

		if(!rag.generate(alphabet_size, state_count, is_dfa, asize, scount, ini, fini, tra))
			return -1;

		count--;
	}
	return (exact_time() - time);
}}}

#define NUM_OPS 50

int main(int argc, char**argv)
{

	int alphabet_size;
	int state_count;

	printf( "\n\n"
		"This program tests, how fast the random generation of n-state-DFAs is.\n"
		"It compares compound-creation (the data structure cashes intermediate\n"
		"results between DFAs) vs. single-creation. For each combination in\n"
		"(state count, alphabet size, {compound,single}), the timing is\n"
		"averaged over %d randomly generated DFAs.\n"
		"\n"
		"The compound timing includes the first-time-generation of non-cached\n"
		"data.\n"
		,
		NUM_OPS);

	for(alphabet_size = 2; alphabet_size <= 6; alphabet_size += 2) {
		printf("\n"
		       "-----------------------------------------------\n"
		       "| asize:  %3d | single   | compound |reduction|\n"
		       "| state count | secs/DFA | secs/DFA |   to    |\n"
		       "|-------------|----------|----------|---------|\n", alphabet_size);
		for(state_count = 3; state_count <= 150; state_count += 3) {

			long double time_compound, time_single;

			// first test 10 single iterations
			time_single = 0;
			for(int i = 0; i < NUM_OPS; i++)
				time_single += generate(alphabet_size, state_count, 1);
			time_single /= NUM_OPS;

			// now test 10 compound iterations
			time_compound = generate(alphabet_size, state_count, NUM_OPS);
			time_compound /= NUM_OPS;
			printf("|        %4d | %8.5Lf | %8.5Lf | %5.1Lf%%  |\n", state_count, time_single, time_compound, time_compound / time_single * 100);
		}
		printf("-----------------------------------------------\n");
	}

	return 0;
}

