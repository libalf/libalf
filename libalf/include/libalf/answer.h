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

#include <ostream>
#include <sstream>

#include <stdint.h>
#include <string>
#include <libalf/serialize.h>

namespace libalf {

/*
 * in general, a possible <answer> should implement the following:
 *
 * as members:
 *   bool operator==(&answer)
 *   bool operator!=(&answer)
 *   void operator=(&answer)
 *
 * additional functions:
 *   std::basic_string<int32_t> serialize(answer a)
 *   bool deserialize(answer & a, serial_stretch & serial)
 *   ostream & operator<<(ostream& os, const answer & a)
 *
 * all the above exist for the builtin-type bool.
 *
 *
 * Following are two custom (example-) types that can be used as
 * answers:
 *
 * first an extended boolean, supporting true, false and unknown.
 *
 * then a type that supports answers as chars between 'a' and 'a'+k,
 * where k is fixed by you. this type can be used to learn
 * moore_machines with an output alphabet of size k.
 */



// example implementation of an extended bool, supporting also the state <unknown>:
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

		inline extended_bool(const extended_bool & e)
		{ value = e.value; };

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

inline std::ostream & operator<<(std::ostream& os, const extended_bool & a)
{{{
	switch(a.value) {
		case extended_bool::EBOOL_FALSE:
			os << "-";
			break;
		case extended_bool::EBOOL_UNKNOWN:
			os << "?";
			break;
		case extended_bool::EBOOL_TRUE:
			os << "+";
			break;
	}
	return os;
}}}



/* implementation of fixed-alphabet-size answer type (e.g. for learning moore_machines<...>),
 * ranging as a char from 'a' to 'a'+size. (thus, size should be no larger than 26, if i
 * counted right :).
 *
 * NOTE:
 *	as the size of the alphabet has to be fixed for the type, not for an instance,
 *	it needs to be passed as a template. currently, this is only possible by defining
 *	a class that provides a function that returns the alphabet size. so, to get a
 *	fixed_count_answer of 'a' .. 'a'+5, define:
 *
 *		class my_size_definition : public fixed_count_answer__size_definition {
 *			virtual int get_size() { return 5; };
 *		};
 *
 *	and use the fixed_count_answer template with this type:
 *
 *		typedef fixed_count_answer<my_size_definition> my_answer_type;
 */
class fixed_count_answer__size_definition {
	public:
		virtual int get_size() = 0;
};

template<typename size_definition>
class fixed_count_answer {
        private:
		size_definition sizedef; // XXX: WTF does static not work (linker error)

                char value;
        public:
                inline fixed_count_answer()
                { value = 0; }

                inline fixed_count_answer(char c)
                { value = c; }

		inline ~fixed_count_answer()
		{ };


                inline static int get_alphabet_size()
                { size_definition sizedef; return sizedef.get_size(); }

                inline void set(char c)
                {{{
                        if(c < 'a')
                                value = 'a';
                        else
                                if(c >= 'a' + sizedef.get_size())
                                        value = 'a' + sizedef.get_size() - 1;
                                else
                                        value = c;
                }}}

                inline char get() const
                { return value; }

                inline void operator=(fixed_count_answer<size_definition> other)
                { this->value = other.value; };

                inline void operator=(int32_t i)
                { set( (char)i ); };

                inline bool operator==(fixed_count_answer<size_definition> other) const
                { return this->value == other.value; }

                inline bool operator==(char c) const
                { return this->value == c; }

                inline operator int32_t() const
                { return (int32_t)value; }
};

template<typename size_definition>
inline std::basic_string<int32_t> serialize(fixed_count_answer <size_definition> a)
{{{
        std::basic_string<int32_t> ret;
        ret += htonl( (int32_t)a );
        return ret;
}}};

template<typename size_definition>
inline bool deserialize(fixed_count_answer<size_definition> & a, serial_stretch & serial)
{{{
        int i;
        if(!::deserialize(i, serial)) return false;
        a = (int32_t)i;
        return true;
}}};

template<typename size_definition>
inline std::ostream & operator<<(std::ostream& os, fixed_count_answer<size_definition> a)
{{{
        os << a.get();

        return os;
}}};



}; // enf of namespace libalf

#endif // __libalf_answer_h__

