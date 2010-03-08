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


#include <stdio.h>

#ifdef _WIN32
#include <winsock.h>
#include <stdint.h>
#else
#include <arpa/inet.h>
#endif

#include <sstream>
#include <string>
#include <iterator>

#include "libalf/statistics.h"

namespace libalf {

using namespace std;

const char * statistic_typenames[] = { "unset", "integer", "double", "bool", "string", NULL };


statistic_data_bad_typecast_e::statistic_data_bad_typecast_e(enum statistic_type vartype, enum statistic_type casttype)
{{{
	snprintf(buf, 128, "bad typecast of generic statistic variable: casting %s to %s.",
			statistic_typenames[vartype], statistic_typenames[casttype] );
}}}
const char * statistic_data_bad_typecast_e::what() const throw()
{{{
	return buf;
}}}

statistic_data::statistic_data()
{{{
	type = UNSET;
}}}
enum statistic_type statistic_data::get_type()
{{{
	return type;
}}}
void statistic_data::set_type(enum statistic_type type)
{{{
	this->type = type;
	switch(type) {
		case INTEGER: i = 0; break;
		case DOUBLE: d = 0.; break;
		case BOOL: b = false; break;
		case STRING: s.clear(); break;
		default: break;
	};
}}}
void statistic_data::unset()
{{{
	type = UNSET;
}}}
void statistic_data::set_integer(int i)
{{{
	type = INTEGER; this->i = i;
}}}
bool statistic_data::get_integer(int & i)
{{{
	i = this->i; return (type == INTEGER);
}}}
void statistic_data::set_double(double d)
{{{
	type = DOUBLE; this->d = d;
}}}
bool statistic_data::get_double(double & d)
{{{
	d = this->d; return (type == DOUBLE);
}}}
void statistic_data::set_bool(bool b)
{{{
	type = BOOL; this->b = b;
}}}
bool statistic_data::get_bool(bool & b)
{{{
	b = this->b; return (type == BOOL);
}}}
void statistic_data::set_string(string s)
{{{
	type = STRING; this->s = s;
}}}
bool statistic_data::get_string(string & s)
{{{
	s = this->s; return (type = STRING);
}}}
string statistic_data::to_string()
{{{
	stringstream str; print(str); return str.str();
}}}
void statistic_data::print(ostream & os)
{{{
	switch(type) {
		case INTEGER: os << i; break;
		case DOUBLE: os << d; break;
		case BOOL: os << (b ? "true" : "false"); break;
		case STRING: os << s ; break;
		default: break;
	};
}}}
basic_string<int32_t> statistic_data::serialize()
{{{
	basic_string<int32_t> ret;

	ret += ::serialize((int)type);

	switch(type) {
		case INTEGER: ret += ::serialize(i); break;
		case DOUBLE: ret += ::serialize(d); break;
		case BOOL: ret += ::serialize(b); break;
		case STRING: ret += ::serialize(s); break;
		default: break;
	};

	return ret;
}}}
bool statistic_data::deserialize(serial_stretch & serial)
{{{
	int t;
	if(!::deserialize(t, serial)) return false;
	type = (enum statistic_type) t;
	switch(type) {
		case INTEGER: if(!::deserialize(i, serial)) return false; break;
		case DOUBLE: if(!::deserialize(d, serial)) return false; break;
		case BOOL: if(!::deserialize(b, serial)) return false; break;
		case STRING: if(!::deserialize(s, serial)) return false; break;
		default: break;
	}
	return true;
}}}

string generic_statistics::to_string()
{{{
	stringstream str;
	print(str);
	return str.str();
}}}
void generic_statistics::print(ostream & os)
{{{
	map<string, statistic_data>::iterator i;

	os << "statistics = { ";

	for(i = this->begin(); i != this->end(); ++i) {
		os << statistic_typenames[i->second.get_type()] << " \"" << i->first << "\"";
		if(i->second.get_type() != UNSET) {
			os << " = ";
			if(i->second.get_type() == STRING) {
				os << '"';
				i->second.print(os);
				os << '"';
			} else {
				i->second.print(os);
			}
		}
		os << "; ";
	}

	os << "}; ";
}}}



timing_statistics::timing_statistics()
{{{
	reset();
}}}
void timing_statistics::reset()
{{{
	user_sec = 0;
	user_usec = 0;
	sys_sec = 0;
	sys_usec = 0;
}}}
basic_string<int32_t> timing_statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += ::serialize(4);
	ret += ::serialize(user_sec);
	ret += ::serialize(user_usec);
	ret += ::serialize(sys_sec);
	ret += ::serialize(sys_usec);

	return ret;
}}}
bool timing_statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	serial_stretch serial;
	serial.current = it;
	serial.limit = limit;

	if(!::deserialize(size, serial)) goto deserialization_failed;
	if(size != 4) goto deserialization_failed;
	if(!::deserialize(user_sec, serial)) goto deserialization_failed;
	if(!::deserialize(user_usec, serial)) goto deserialization_failed;
	if(!::deserialize(sys_sec, serial)) goto deserialization_failed;
	if(!::deserialize(sys_usec, serial)) goto deserialization_failed;

	it = serial.current;
	return true;

deserialization_failed:
	reset();
	it = serial.current;
	return false;
}}}

// -------------------------------------------------------------------------------------------------- //
// EVERYTHING BELOW THIS LINE IS OBSOLETE AND WILL BE REMOVED AFTER ALL INTERFACES HAVE BEEN CHANGED. //
// -------------------------------------------------------------------------------------------------- //


query_statistics::query_statistics()
{{{
	reset();
}}}
void query_statistics::reset()
{{{
	membership = 0;
	uniq_membership = 0;
	equivalence = 0;
}}}
basic_string<int32_t> query_statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += htonl(3);
	ret += htonl(membership);
	ret += htonl(uniq_membership);
	ret += htonl(equivalence);

	return ret;
}}}
bool query_statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	if(it == limit) goto deserialization_failed;

	// data size
	size = ntohl(*it);
	if(size != 3) goto deserialization_failed;

	// membership
	it++; if(limit == it) goto deserialization_failed;
	membership = ntohl(*it);
	// uniq_membership
	it++; if(limit == it) goto deserialization_failed;
	uniq_membership = ntohl(*it);
	// equivalence
	it++; if(limit == it) goto deserialization_failed;
	equivalence = ntohl(*it);

	return true;

deserialization_failed:
	reset();
	return false;
}}}


memory_statistics::memory_statistics()
{{{
	reset();
}}}
void memory_statistics::reset()
{{{
	bytes = 0;
	members = 0;
	words = 0;
	upper_table = 0;
	lower_table = 0;
	columns = 0;
}}}
basic_string<int32_t> memory_statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += htonl(6);
	ret += htonl(bytes);
	ret += htonl(members);
	ret += htonl(words);
	ret += htonl(upper_table);
	ret += htonl(lower_table);
	ret += htonl(columns);

	return ret;
}}}
bool memory_statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;

	if(it == limit) goto deserialization_failed;

	// data size
	size = ntohl(*it);
	if(size != 6) goto deserialization_failed;

	// bytes
	it++; if(limit == it) goto deserialization_failed;
	bytes = ntohl(*it);
	// members
	it++; if(limit == it) goto deserialization_failed;
	members = ntohl(*it);
	// words
	it++; if(limit == it) goto deserialization_failed;
	words = ntohl(*it);
	// upper_table
	it++; if(limit == it) goto deserialization_failed;
	upper_table = ntohl(*it);
	// lower_table
	it++; if(limit == it) goto deserialization_failed;
	lower_table = ntohl(*it);
	// columns
	it++; if(limit == it) goto deserialization_failed;
	columns = ntohl(*it);

	return true;

deserialization_failed:
	reset();
	return false;
}}}


void statistics::reset()
{{{
	queries.reset();
	memory.reset();
	time.reset();
}}}
basic_string<int32_t> statistics::serialize()
{{{
	basic_string<int32_t> ret;

	ret += 0; // length field, filled in later.

	ret += queries.serialize();
	ret += memory.serialize();
	ret += time.serialize();

	ret[0] = htonl( ret.length() - 1 );
	return ret;
}}}
bool statistics::deserialize(basic_string<int32_t>::iterator & it, basic_string<int32_t>::iterator limit)
{{{
	int size;
	basic_string<int32_t>::iterator end;

	if(it == limit)
		goto deserialization_failed;

	// data size
	size = ntohl(*it);
	it++;
	end = it;
	end += size;

	if(!queries.deserialize(it, limit)) goto deserialization_failed;
	if(!memory.deserialize(it, limit)) goto deserialization_failed;
	if(!time.deserialize(it, limit)) goto deserialization_failed;

	if(it != end) goto deserialization_failed;

	return true;

deserialization_failed:
	reset();
	return false;
}}}

}; // end namespace libalf
