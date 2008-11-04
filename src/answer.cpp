/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include "libalf/answer.h"

namespace libalf {

bool extended_bool::operator==(extended_bool &other)
{{{
	return this->value == other.value;
}}}

bool operator==(extended_bool a, extended_bool b)
{{{
	return a.value == b.value;
}}}


bool extended_bool::operator!=(extended_bool &other)
{{{
	return this->value != other.value;
}}}

void extended_bool::operator=(extended_bool &other)
{{{
	value = other.value;
}}}

bool extended_bool::operator>(extended_bool &other)
{{{
	return( ((int)this->value) > ((int)other.value));
}}}

bool extended_bool::operator==(bool other)
{{{
	  if(other)
		  return (value == EBOOL_TRUE);
	  else
		  return (value == EBOOL_FALSE);
}}}

bool extended_bool::operator>(bool other)
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

}; // end of namespace libalf

