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
 *           and David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifdef _WIN32
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif

#include <stdio.h>

#include <libmVCA/pushdown.h>

using namespace std;

namespace libmVCA {

pushdown_alphabet::pushdown_alphabet()
{ alphabet_size = 0; };

pushdown_alphabet::pushdown_alphabet(int alphabet_size)
{ set_alphabet_size(alphabet_size); };

pushdown_alphabet::~pushdown_alphabet()
{ /* nothing */ };

void pushdown_alphabet::set_alphabet_size(int alphabet_size)
{{{
	if(alphabet_size < 0)
		alphabet_size = 0;
	directions.resize(alphabet_size, DIR_STAY);
	this->alphabet_size = alphabet_size;
}}}

int pushdown_alphabet::get_alphabet_size() const
{ return this->alphabet_size; };

enum pushdown_direction pushdown_alphabet::get_direction(int sigma) const
{{{
	if(sigma >= 0 && sigma < alphabet_size)
		return directions[sigma];
	else
		return DIR_INDEFINITE;
}}}

bool pushdown_alphabet::set_direction(int sigma, enum pushdown_direction direction)
{{{
	if(sigma >= 0 && sigma < alphabet_size && (direction == DIR_UP || direction == DIR_STAY || direction == DIR_DOWN)) {
		directions[sigma] = direction;
		return true;
	} else {
		return false;
	}
}}}

int pushdown_alphabet::prefix_countervalue(list<int>::const_iterator word, list<int>::const_iterator limit, int initial_countervalue) const
{{{
	for(/* nothing */; word != limit; ++word) {
		enum pushdown_direction d = get_direction(*word);
		if(d != DIR_INDEFINITE) {
			initial_countervalue += (int)d;
			if(initial_countervalue < 0)
				break;
		} else {
			initial_countervalue = -1;
			break;
		}
	}
	return initial_countervalue;
}}}

std::basic_string<int32_t> pushdown_alphabet::serialize() const
{{{
	return ::serialize(alphabet_size) + ::serialize(directions);
}}}

bool pushdown_alphabet::deserialize(::serial_stretch serial)
{{{
	clear();

	if(!::deserialize(alphabet_size, serial)) return false;
	if(!::deserialize(directions, serial)) return false;

	return true;
}}}

string pushdown_alphabet::to_string() const
{{{
	string ret;
	char buf[128];

	snprintf(buf, 128, "pushdown alphabet of size %d\n", alphabet_size);
	ret += buf;

	for(int i = 0; i < alphabet_size; ++i) {
		snprintf(buf, 128, "  %d :: %d\n", i, directions[i]);
		ret += buf;
	}
	ret += "\n";

	return ret;
}}}

} // end of namespace libmVCA

