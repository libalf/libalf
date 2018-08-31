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
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *                and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_statistics_h__
# define __libalf_statistics_h__

#include <stdint.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <iterator>
#include <iostream>
#include <ostream>
#include <sstream>

#include <exception>

#define __helper__serialize_only_forward_declarations__
#include <libalf/serialize.h>

namespace libalf {

enum statistic_type {
	UNSET = 0,
	INTEGER = 1,
	DOUBLE = 2,
	BOOL = 3,
	STRING = 4
};

extern const char * statistic_typenames[];

/** exception that will be thrown if a statistic_data is casted to the wrong type */
class statistic_data_bad_typecast_e : public std::exception
{
	private:
		char buf[128];
	public:
		statistic_data_bad_typecast_e(enum statistic_type vartype, enum statistic_type casttype);
		virtual const char * what() const throw();
};

/** generic data type
 *
 *  this can be one of int, double, bool or string, depending on what is assigned to it.
 */
class statistic_data
{
	private:
		enum statistic_type type;

		union {
			int i;
			double d;
			bool b;
		};
		std::string s; // as string has a constructur, we can't put it in a union.

	public:
		statistic_data();

		std::string to_string() const;
		void print(std::ostream & os) const;

		std::basic_string<int32_t> serialize() const;
		bool deserialize(serial_stretch & serial);

		enum statistic_type get_type() const;
		void set_type(enum statistic_type type);

		void unset();

		void set_integer(int i);
		void set_double(double d);
		void set_bool(bool b);
		void set_string(std::string s);

		bool get_integer(int & i) const;
		bool get_double(double & d) const;
		bool get_bool(bool & b) const;
		bool get_string(std::string & s) const;

		inline statistic_data & operator=(const int & i)         { set_integer(i); return *this; };
		inline statistic_data & operator=(const double & d)      { set_double(d);  return *this; };
		inline statistic_data & operator=(const bool & b)        { set_bool(b);    return *this; };
		inline statistic_data & operator=(const std::string & s) { set_string(s);  return *this; };
		inline statistic_data & operator=(const char * c)        { set_string(c);  return *this; };

		inline operator int()	      { if(type == INTEGER) return i; else throw statistic_data_bad_typecast_e(type, INTEGER); };
		inline operator double()      { if(type == DOUBLE)  return d; else throw statistic_data_bad_typecast_e(type, DOUBLE);  };
		inline operator bool()	      { if(type == BOOL)    return b; else throw statistic_data_bad_typecast_e(type, BOOL);    };
		inline operator std::string() { if(type == STRING)  return s; else throw statistic_data_bad_typecast_e(type, STRING);  };
};

// required for generic serialisation:
inline std::basic_string<int32_t> serialize(const statistic_data & s)		{ return s.serialize(); }
inline bool deserialize(statistic_data & into, serial_stretch & serial)		{ return into.deserialize(serial); };


/** anymap-style statistic collection
 *
 *  collection of statistic data that is indexed by name.
 *
 *  convention for names: (optional, but a good idea)
 *	- avoid abbreviations
 *	- avoid spaces
 *	- stick to lower case
 *	- use period ``.'' to impose tree-structure
 *	- basic tree-branches:
 *	   `time'		used for processor time resource consumed e.g. by algorithm
 *	   `memory'		used for memory resource consumed
 *	   `size'		size-specific entries (e.g. table-size, knowledge size)
 */
class generic_statistics : public std::map<std::string, statistic_data>
{
	public: // types
		typedef std::map<std::string, statistic_data>::iterator iterator;
		typedef std::map<std::string, statistic_data>::const_iterator const_iterator;
	public: // methods
		std::string to_string() const;
		void print(std::ostream & os) const;

		inline void remove_property(const std::string & key)				{ this->erase(key); };

		inline void unset_property(const std::string & key)				{ (*this)[key].unset(); };

		inline void set_integer_property(const std::string & key, int value)		{ (*this)[key].set_integer(value); }
		inline bool get_integer_property(const std::string & key, int & into)		{ return (*this)[key].get_integer(into); }

		inline void set_double_property(const std::string & key, double value)		{ (*this)[key].set_double(value); }
		inline bool get_double_property(const std::string & key, double & into)		{ return (*this)[key].get_double(into); }

		inline void set_bool_property(const std::string & key, bool value)		{ (*this)[key].set_bool(value); }
		inline bool get_bool_property(const std::string & key, bool & into)		{ return (*this)[key].get_bool(into); }

		inline void set_string_property(const std::string & key, std::string value)	{ (*this)[key].set_string(value); }
		inline bool get_string_property(const std::string & key, std::string & into)	{ return (*this)[key].get_string(into); }
};

// -------------------------------------------------------------------------------------------------- //
// EVERYTHING BELOW THIS LINE IS OBSOLETE AND WILL BE REMOVED AFTER ALL INTERFACES HAVE BEEN CHANGED. //
// -------------------------------------------------------------------------------------------------- //

class timing_statistics {
	public:	// data
		int32_t user_sec;
		int32_t user_usec;
		int32_t sys_sec;
		int32_t sys_usec;
	public: // methods
		timing_statistics();
		void reset();
		std::basic_string<int32_t> serialize() const;
		bool deserialize(serial_stretch & serial);
} __attribute__((deprecated));

class query_statistics {
	public:	// data
		int32_t membership;
		int32_t uniq_membership;
		int32_t equivalence;
	public:	// methods
		query_statistics();
		void reset();
		std::basic_string<int32_t> serialize();
		bool deserialize(std::basic_string<int32_t>::const_iterator & it, std::basic_string<int32_t>::const_iterator limit);
} __attribute__((deprecated));

class memory_statistics {
	public:	// data
		int32_t bytes;		// bytes of algorithms data structure
		int32_t members;	// number of membership data
		int32_t words;		// number of words in table
		int32_t upper_table;	// size of upper table (if appropriate)
		int32_t lower_table;	// size of lower table (if appropriate)
		int32_t columns;	// columns (if appropriate)
	public:	// methods
		memory_statistics();
		void reset();
		std::basic_string<int32_t> serialize();
		bool deserialize(std::basic_string<int32_t>::const_iterator & it, std::basic_string<int32_t>::const_iterator limit);
} __attribute__((deprecated));

class statistics {
	public: // data
		query_statistics queries;
		memory_statistics memory;
		timing_statistics time;

	public: // methods
		void reset();
		std::basic_string<int32_t> serialize();
		bool deserialize(std::basic_string<int32_t>::const_iterator & it, std::basic_string<int32_t>::const_iterator limit);
} __attribute__((deprecated));

}; // end namespace libalf

#endif

