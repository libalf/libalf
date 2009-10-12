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

#ifndef __liblangen_langen_h__
# define __liblangen_langen_h__

#include <liblangen/prng.h>
#include <liblangen/dfa_enumerator.h>
#include <liblangen/dfa_randomgenerator.h>
#include <liblangen/nfa_randomgenerator.h>
#include <liblangen/regex_randomgenerator.h>

namespace liblangen {

using namespace std;

const char* liblangen_version();

}; // end of namespace liblangen

#endif // __liblangen_langen_h__

