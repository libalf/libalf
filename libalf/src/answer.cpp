/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#include <stdint.h>

#include "libalf/answer.h"

namespace libalf {

bool __attribute__((const)) extended_bool::operator==(extended_bool &other)
{{{
	return this->value == other.value;
}}}

bool __attribute__((const)) operator==(extended_bool a, extended_bool b)
{{{
	return a.value == b.value;
}}}


bool __attribute__((const)) extended_bool::operator!=(extended_bool &other)
{{{
	return this->value != other.value;
}}}

void extended_bool::operator=(extended_bool &other)
{{{
	value = other.value;
}}}

bool __attribute__((const)) extended_bool::operator>(extended_bool &other)
{{{
	return( ((int)this->value) > ((int)other.value));
}}}

bool __attribute__((const)) extended_bool::operator==(bool other)
{{{
	  if(other)
		  return (value == EBOOL_TRUE);
	  else
		  return (value == EBOOL_FALSE);
}}}

bool __attribute__((const)) extended_bool::operator>(bool other)
{{{
	  if(other)
		  return false;
	  else
		  return (value > EBOOL_FALSE);
}}}

void extended_bool::operator=(bool other)
{{{
	if(other)
		value = EBOOL_TRUE;
	else
		value = EBOOL_FALSE;
}}}

__attribute__((const)) extended_bool::operator int32_t()
{{{
	return (int32_t)value;
}}}

void extended_bool::operator=(int32_t other)
{{{
	value = (enum e_extended_bool)other;
}}}

}; // end of namespace libalf

