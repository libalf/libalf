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
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

/*
 * RegEx Random Generator, similar to the one described in section 4.3 of
 * "F. Denis, A. Lemay and A. Terlutte - Learning regular languages using RFSAs"
 */

#include <set>
#include <map>
#include <string>

#ifndef __liblangen_regex_randomgenerator_h__
# define __liblangen_regex_randomgenerator_h__

namespace liblangen {

using namespace std;

class regex_randomgenerator {
	private:

	public:
		regex_randomgenerator();

		// create a random regular expression
		// the sum of all probablities may be != 1, but >= 0. the algorithm will normalize.
		string generate(int alphabet_size, int num_op, float p_concat, float p_union, float p_star);
		// this version expects normalized probablilities
		string normalized_generate(int & alphabet_size, int num_op, float & p_concat, float & p_union, float & p_star);
};

}; // end of namespace liblangen

#endif


