/* $Id$
 * vim: fdm=marker
 *
 * liblangen (LANguageGENerator)
 * Pseudo Random Number Generator:
 * functions for generation of random numbers of different kind.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <set>
#include <map>
#include <list>
#include <vector>

#include <gmpxx.h> // GNU Multiple Precision Arithmetic Library, C++ interface

#ifndef __liblangen_prng_h__
# define __liblangen_prng_h__

namespace liblangen {
namespace prng {

	bool seed_prng();

	// returns a random int in [0,limit):
	int random_int(int limit);

	// returns a random float in [0,1):
	float random_float();

	// will return a random float [0,1]:
	float random_float1();

	// returns a random mpz_class (int) in [0,limit):
	void random_mpz_class(mpz_class & into, mpz_class & limit);

}; // end of namespace prng
}; // end of namespace liblangen

#endif

