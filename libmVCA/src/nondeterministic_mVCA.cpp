/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libmVCA.
 *
 * libmVCA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libmVCA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmVCA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <libmVCA/mVCA.h>
#include <libmVCA/nondeterministic_mVCA.h>

namespace libmVCA {

using namespace std;


set<int> nondeterministic_mVCA::transition(const set<int> & from, int & m, int label)
{{{
	enum pushdown_direction dir;
	set<int> ret;

	dir = this->alphabet.get_direction(label);

	if(dir != DIR_INDEFINITE) {
		ret = delta_function[m].transmute(from, label);
		m += dir;
	} else {
		m = -1;
	}

	return ret;
}}}

string nondeterministic_mVCA::generate_dotfile()
{
	
}

basic_string<int32_t> nondeterministic_mVCA::serialize_derivate()
{
	
}
bool nondeterministic_mVCA::deserialize_derivate(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	
}


} // end of namespace libmVCA

