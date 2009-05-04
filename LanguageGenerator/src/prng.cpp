/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * Pseudo Random Number Generator:
 * functions for generation of random numbers of different kind.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

#include <gmp.h>
#include <gmpxx.h>

static bool seeded = false;
static gmp_randstate_t grstate;

namespace LanguageGenerator {
namespace prng {

	bool seed_prng()
	{{{
		if(!seeded) {
			// seed PRNG: init GMP random number generator state
			unsigned long int gmp_seed;
			unsigned int n_seed;
			int ran;

			ran = open("/dev/urandom", O_RDONLY);
			if(ran < 0)
				return false;
			if(read(ran, &gmp_seed, sizeof(gmp_seed)) != sizeof(gmp_seed)) {
				close(ran);
				return false;
			}
			if(read(ran, &n_seed, sizeof(n_seed)) != sizeof(n_seed)) {
				close(ran);
				return false;
			}
			close(ran);

			gmp_randinit_default(grstate);
			gmp_randseed_ui(grstate, gmp_seed);

			srand(n_seed);

			seeded = true;
		}
		return true;
	}}}

	int random_int(int limit)
	// will return a random integer in [0,limit)
	{{{
		float t = RAND_MAX;
		while(t == RAND_MAX)
			t = rand();
		t /= RAND_MAX;
		t *= limit;
		return (int)t;
	}}}

	float random_float()
	// will return a random float [0,1)
	{{{
		return ((float)rand()) / ((float)RAND_MAX+1);
	}}}

	float random_float1()
	// will return a random float [0,1]
	{{{
		return ((float)rand()) / ((float)RAND_MAX);
	}}}

	void random_mpz_class(mpz_class & into, mpz_class & limit)
	{{{
		mpz_urandomm(into.get_mpz_t(), grstate, limit.get_mpz_t());
	}}}

}; // end of namespace prng;
}; // end of namespace LanguageGenerator;

