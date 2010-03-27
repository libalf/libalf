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
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_answer_h__
# define __libalf_answer_h__

#include <stdint.h>
#include <string>
#include <libalf/serialize.h>

namespace libalf {

// a possible <answer> has to implement the following:
//
// bool operator==(&answer)
// bool operator!=(&answer)
// bool operator>(&answer) , where true > possibly true > unknown > possibly false > false
// void operator=(&answer)
//
// bool operator==(bool)
// bool operator!=(bool)
// bool operator>(bool)
// void operator=(bool)
//
// operator int32_t()
// void operator=(int32_t)
//
// has to be castable to int32_t
//
// all the above are valid for the builtin-type bool.


// an example implementation:


class extended_bool {
	public:
		enum e_extended_bool {
			EBOOL_FALSE = 0,
			EBOOL_UNKNOWN = 1,
			EBOOL_TRUE = 2
		};

		enum e_extended_bool value;

		inline extended_bool()
		{ value = EBOOL_UNKNOWN; };

		inline extended_bool(bool val)
		{ value = (val ? EBOOL_TRUE : EBOOL_FALSE); };

		inline bool __attribute__((const)) valid() const
		{{{
			  return (value == EBOOL_FALSE || value == EBOOL_UNKNOWN || value == EBOOL_TRUE);
		}}}

		inline bool __attribute__((const)) operator!=(const extended_bool & other) const
		{{{
			return this->value != other.value;
		}}}

		inline void operator=(const extended_bool & other)
		{{{
			value = other.value;
		}}}

		inline bool __attribute__((const)) operator>(const extended_bool & other) const
		{{{
			return( ((int)this->value) > ((int)other.value));
		}}}

		inline bool __attribute__((const)) operator==(bool other) const
		{{{
			  if(other)
				  return (value == EBOOL_TRUE);
			  else
				  return (value == EBOOL_FALSE);
		}}}

		inline bool __attribute__((const)) operator>(bool other) const
		{{{
			  if(other)
				  return false;
			  else
				  return (value > EBOOL_FALSE);
		}}}

		inline void operator=(bool other)
		{{{
			if(other)
				value = EBOOL_TRUE;
			else
				value = EBOOL_FALSE;
		}}}

		inline operator int32_t() const
		{{{
			return (int32_t)value;
		}}}

		inline void operator=(int32_t other)
		{{{
			value = (enum e_extended_bool)other;
		}}}
};

inline bool __attribute__((const)) operator==(extended_bool a, extended_bool b)
{{{
	return a.value == b.value;
}}}

inline std::basic_string<int32_t> serialize(extended_bool e)
{{{
	std::basic_string<int32_t> ret;
	ret += htonl((int32_t)e);
	return ret;
}}}

inline bool deserialize(extended_bool & e, serial_stretch & serial)
{{{
	int i;
	if(!::deserialize(i, serial)) return false;
	e = ( (int32_t)i );
	return true;
}}}

}; // enf of namespace libalf

#endif // __libalf_answer_h__

