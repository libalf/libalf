/* vim: fdm=syntax foldlevel=1 foldnestmax=2
 * $Id$
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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

/*
 * Pseudo Random Number Generator:
 * functions for generation of random numbers of different kind.
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

namespace liblangen {
namespace prng {

	bool seed_prng()
	{
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
	}

	int random_int(int limit)
	// will return a random integer in [0,limit)
	{
		float t = RAND_MAX;
		while(t == RAND_MAX)
			t = rand();
		t /= RAND_MAX;
		t *= limit;
		return (int)t;
	}

	float random_float()
	// will return a random float [0,1)
	{
		float r = rand();
		r /= ((float)RAND_MAX)+1;
		return r;
	}

	float random_float1()
	// will return a random float [0,1]
	{
		return ((float)rand()) / ((float)RAND_MAX);
	}

	void random_mpz_class(mpz_class & into, mpz_class & limit)
	{
		mpz_urandomm(into.get_mpz_t(), grstate, limit.get_mpz_t());
	}

}; // end of namespace prng;
}; // end of namespace liblangen;

