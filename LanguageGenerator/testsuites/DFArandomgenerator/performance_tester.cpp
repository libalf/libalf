/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * performance_tester: speed tests for DFA random generator.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include <LanguageGenerator/DFArandomgenerator.h>
#include <LanguageGenerator/automaton_constructor.h>

using namespace LanguageGenerator;
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

	time = exact_time();
	while(count > 0) {
		DFArandomgenerator rag;
		basic_automaton_holder automaton;

		if(!rag.generate(alphabet_size, state_count, automaton)) {
			return -1;
		}

		count--;
	}
	return (exact_time() - time);
}}}

#define NUM_OPS 50

int main(int argc, char**argv)
{

	int alphabet_size;
	int state_count;

	for(alphabet_size = 2; alphabet_size < 6; alphabet_size++) {
		printf("ASIZE %2d            compound , single\n", alphabet_size);
		for(state_count = 2; state_count < 100; state_count++) {

			long double time_compound, time_single;

			// first test 10 single iterations
			time_compound = 0;
			for(int i = 0; i < NUM_OPS; i++)
				time_compound += generate(alphabet_size, state_count, 1);
			time_compound /= NUM_OPS;

			// now test 10 compound iterations
			time_single = generate(alphabet_size, state_count, NUM_OPS);
			time_single /= NUM_OPS;
			printf("state count %03d - %10.6Lf , %10.6Lf  -  delta: %10.6Lf\n", state_count, time_compound, time_single, time_single - time_compound);
		}
	}

	return 0;
}

