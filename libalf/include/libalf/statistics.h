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

#include <map>
#include <string>
#include <iterator>
#include <iostream>
#include <ostream>
#include <sstream>

#include <exception>

#include <libalf/serialize.h>

namespace libalf {

using namespace std;


enum statistic_type {
	UNSET = 0,
	INTEGER = 1,
	DOUBLE = 2,
	BOOL = 3,
	STRING = 4
};

extern const char * statistic_typenames[];

class statistic_data_bad_typecast_e : public exception {
	private:
		enum statistic_type vartype;
		enum statistic_type casttype;
	public:
		statistic_data_bad_typecast_e(enum statistic_type vartype, enum statistic_type casttype);
		virtual const char * what() const throw();
		string get_type_information();
};

class statistic_data {
	private:
		enum statistic_type type;

		union {
			int i;
			double d;
			bool b;
		};
		string s; // can't put this into the union cause its a class

	public:
		statistic_data();

		string to_string();
		void print(ostream & os);

		basic_string<int32_t> serialize();
		bool deserialize(serial_stretch & serial);

		enum statistic_type get_type();
		void set_type(enum statistic_type type);

		void unset();

		void set_integer(int i);
		void set_double(double d);
		void set_bool(bool b);
		void set_string(string s);

		bool get_integer(int & i);
		bool get_double(double & d);
		bool get_bool(bool & b);
		bool get_string(string & s);

		inline statistic_data & operator=(const int & i)    { set_integer(i); return *this; };
		inline statistic_data & operator=(const double & d) { set_double(d);  return *this; };
		inline statistic_data & operator=(const bool & b)   { set_bool(b);    return *this; };
		inline statistic_data & operator=(const string & s) { set_string(s);  return *this; };
		inline statistic_data & operator=(const char * c)   { set_string(c);  return *this; };

		inline operator int()	 throw (statistic_data_bad_typecast_e) { if(type == INTEGER) return i; else throw statistic_data_bad_typecast_e(type, INTEGER); };
		inline operator double() throw (statistic_data_bad_typecast_e) { if(type == DOUBLE)  return d; else throw statistic_data_bad_typecast_e(type, DOUBLE);  };
		inline operator bool()	 throw (statistic_data_bad_typecast_e) { if(type == BOOL)    return b; else throw statistic_data_bad_typecast_e(type, BOOL);    };
		inline operator string() throw (statistic_data_bad_typecast_e) { if(type == STRING)  return s; else throw statistic_data_bad_typecast_e(type, STRING);  };
};

// required for generic serialisation:
inline basic_string<int32_t> serialize(statistic_data & s)			{ return s.serialize(); }
inline bool deserialize(statistic_data & into, serial_stretch & serial)		{ return into.deserialize(serial); };


class generic_statistics : public map<string, statistic_data> {
	public:
		string to_string();
		void print(ostream & os);

		inline void remove_property(const string & key)					{ this->erase(key); };

		inline void unset_property(const string & key)					{ (*this)[key].unset(); };

		inline void set_integer_property(const string & key, int value)			{ (*this)[key].set_integer(value); }
		inline bool get_integer_property(const string & key, int & into)		{ return (*this)[key].get_integer(into); }

		inline void set_double_property(const string & key, double value)		{ (*this)[key].set_double(value); }
		inline bool get_double_property(const string & key, double & into)		{ return (*this)[key].get_double(into); }

		inline void set_bool_property(const string & key, bool value)			{ (*this)[key].set_bool(value); }
		inline bool get_bool_property(const string & key, bool & into)			{ return (*this)[key].get_bool(into); }

		inline void set_string_property(const string & key, string value)		{ (*this)[key].set_string(value); }
		inline bool get_string_property(const string & key, string & into)		{ return (*this)[key].get_string(into); }
};


class timing_statistics {
	public:	// data
		int32_t user_sec;
		int32_t user_usec;
		int32_t sys_sec;
		int32_t sys_usec;
	public: // methods
		timing_statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

// -------------------------------------------------------------------------------------------------- //
// EVERYTHING BELOW THIS LINE IS OBSOLETE AND WILL BE REMOVED AFTER ALL INTERFACES HAVE BEEN CHANGED. //
// -------------------------------------------------------------------------------------------------- //

class query_statistics {
	public:	// data
		int32_t membership;
		int32_t uniq_membership;
		int32_t equivalence;
	public:	// methods
		query_statistics();
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

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
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

class statistics {
	public: // data
		query_statistics queries;
		memory_statistics memory;
		timing_statistics time;

	public: // methods
		void reset();
		basic_string<int32_t> serialize();
		bool deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit);
};

}; // end namespace libalf

#endif

