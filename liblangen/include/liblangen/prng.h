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
 * (c) 2008,2009 by David R. Piegdon, Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

/*
 * Pseudo Random Number Generator:
 * functions for generation of random numbers of different kind.
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

