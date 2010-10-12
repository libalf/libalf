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
 * (c) 2010 David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <string>
#include <arpa/inet.h>
#include <libalf/serialize.h>

using namespace std;

// the answer-class we want to learn will be based on the alphabet {a,b,c} using a char.
class my_answer {
	private:
		static const int alphabet_size = 3;
		char value;
	public:
		inline my_answer()
		{ value = 0; }

		inline my_answer(char c)
		{ value = c; }

		inline static int get_alphabet_size()
		{ return alphabet_size; }

		inline void set(char c)
		{{{
			if(c < 'a')
				value = 'a';
			else
				if(c >= 'a' + alphabet_size)
					value = 'a' + alphabet_size - 1;
				else
					value = c;
		}}}

		inline char get() const
		{ return value; }

		inline void operator=(my_answer other)
		{ this->value = other.value; };

		inline void operator=(int32_t i)
		{ set( (char)i ); };

		inline bool operator==(my_answer other) const
		{ return this->value == other.value; }

		inline bool operator==(char c) const
		{ return this->value == c; }

		inline operator int32_t() const
		{ return (int32_t)value; }
};

inline std::basic_string<int32_t> serialize(my_answer a)
{
	std::basic_string<int32_t> ret;
	ret += htonl( (int32_t)a );
	return ret;
};

inline bool deserialize(my_answer & a, serial_stretch & serial)
{
	int i;
	if(!::deserialize(i, serial)) return false;
	a = (int32_t)i;
	return true;
};

inline ostream & operator<<(ostream& os, const my_answer & a)
{
	os << a.get();

	return os;
};

