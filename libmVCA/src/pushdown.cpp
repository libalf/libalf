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

#ifdef _WIN32
# include <winsock.h>
# include <stdio.h>
#else
# include <arpa/inet.h>
#endif

#include <libmVCA/pushdown.h>

namespace libmVCA {

pushdown_alphabet::pushdown_alphabet()
{ alphabet_size = 0; };

pushdown_alphabet::pushdown_alphabet(int alphabet_size)
{ this->alphabet_size = alphabet_size; };

pushdown_alphabet::~pushdown_alphabet()
{ /* nothing */ };

void pushdown_alphabet::set_alphabet_size(int alphabet_size)
{{{
	if(alphabet_size < 0)
		alphabet_size = 0;
	// remove obsolete direction data,
	for(int sigma = alphabet_size; sigma < this->alphabet_size; sigma++)
		this->directions.erase(sigma);
	// initialise new direction data
	for(int sigma = this->alphabet_size; sigma < alphabet_size; sigma++)
		this->directions[sigma] = dir_stay;
	this->alphabet_size = alphabet_size;
}}}

int pushdown_alphabet::get_alphabet_size()
{ return this->alphabet_size; };

enum pushdown_direction pushdown_alphabet::get_direction(int sigma)
{{{
	if(sigma >= 0 && sigma < alphabet_size)
		return this->directions[sigma];
	else
		return dir_indefinite;
}}}

bool pushdown_alphabet::set_direction(int sigma, enum pushdown_direction direction)
{{{
	if(sigma >= 0 && sigma < alphabet_size && direction != dir_indefinite) {
		this->directions[sigma] = direction;
		return true;
	} else {
		return false;
	}
}}}

std::basic_string<int32_t> pushdown_alphabet::serialize()
{{{
	basic_string<int32_t> ret;

	// stream length, will be filled in later.
	ret += 0;

	ret += htonl(this->alphabet_size);

	for(int sigma = 0; sigma < this->alphabet_size; sigma++)
		ret += htonl(this->directions[sigma]);

	ret[0] = htonl(ret.length() - 1);

	return ret;
}}}

bool pushdown_alphabet::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	clear();

	if(it == limit) return false;
	size = ntohl(*it);
	++it;
	if(size <= 0 || limit == it) return false;

	this->alphabet_size = ntohl(*it);
	if(this->alphabet_size < 0) goto deserialization_failed;

	for(int sigma = 0; sigma < this->alphabet_size; sigma++) {
		++it; --size;
		if(size <= 0 || limit == it) goto deserialization_failed;
		enum pushdown_direction d;
		d = (enum pushdown_direction) ntohl(*it);
		if(d != dir_up && d != dir_stay && d != dir_down)
			goto deserialization_failed;
		this->directions[sigma] = d;
	}

	if(size == 0)
		return true;

deserialization_failed:
	return false;
}}}

} // end of namespace libmVCA

