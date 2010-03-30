/* $Id$
 * vim: fdm=marker
 *
 * serialize functions
 *
 * this file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * this file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2010 David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __helper_serialize_h__body__

// NOTE: __helper__serialize_h__only_forward_declarations__ may be defined by
// the including party to only include forward declarations.
// in this case, serialize.h needs to be included again later on.

// __helper_serialize_h__forward__ is defined if forward declarations were included already.
# ifndef __helper_serialize_h__forward__
#  define __helper_serialize_h__forward__

// headers that are needed for forward declaration:
#  include <string>
#  include <list>
#  include <set>
#  include <map>
#  include <iterator>
#  include <vector>
#  include <stdint.h>

class  serial_stretch {
	public:
		std::basic_string<int32_t>::const_iterator current;
		std::basic_string<int32_t>::const_iterator limit;
	public:
		inline serial_stretch()
		{ };

		inline serial_stretch(std::basic_string<int32_t>::const_iterator current, std::basic_string<int32_t>::const_iterator limit)
		{ this->current = current; this->limit = limit; };

		inline serial_stretch(std::basic_string<int32_t> & serial)
		{ init(serial); };

		inline void init(std::basic_string<int32_t> & serial)
		{ this->current = serial.begin(); this->limit = serial.end(); };

		inline bool empty()
		{ return (current == limit); };

		inline serial_stretch & operator++()
		{ current++; return *this; };

		inline serial_stretch operator++(int __attribute__ ((__unused__)) foo)
		{ serial_stretch s; s = *this; operator++(); return s; };

		inline int operator*()
		{ return *current; };
};

// forward declarations
inline                                  std::basic_string<int32_t> serialize(int a); // works for int, unsinged int, char and bool
inline                                  std::basic_string<int32_t> serialize(double & a); // has to be non-const reference, otherwise it is ambiguous with (int)
inline                                  bool deserialize(int & into, serial_stretch & serial);
inline                                  bool deserialize(unsigned int & into, serial_stretch & serial);
inline                                  bool deserialize(bool & into, serial_stretch & serial);
inline					bool deserialize(char & into, serial_stretch & serial);
inline					bool deserialize(double & into, serial_stretch & serial);

template <typename S, typename T>       std::basic_string<int32_t> serialize(const std::pair<S, T> & p);
template <typename S, typename T>       bool deserialize(std::pair<S, T> & p, serial_stretch & serial);
template <typename S>                   std::basic_string<int32_t> serialize(const std::list<S> & l);
template <typename S>                   bool deserialize(std::list<S> & l, serial_stretch & serial);
template <typename S>                   std::basic_string<int32_t> serialize(const std::vector<S> & v);
template <typename S>                   bool deserialize(std::vector<S> & v, serial_stretch & serial);
template <typename S>                   std::basic_string<int32_t> serialize(const std::set<S> & s);
template <typename S>                   bool deserialize(std::set<S> & s, serial_stretch & serial);
template <typename S, typename T>       std::basic_string<int32_t> serialize(const std::map<S, T> & m);
template <typename S, typename T>       bool deserialize(std::map<S, T> & m, serial_stretch & serial);
template <typename S, typename T>	std::basic_string<int32_t> serialize(const std::multimap<S, T> & m);
template <typename S, typename T>	bool deserialize(std::multimap<S, T> & m, serial_stretch & serial);
template <typename S>			std::basic_string<int32_t> serialize(const std::basic_string<S> & s);
template <typename S>			bool deserialize(std::basic_string<S> & s, serial_stretch & serial);

# endif // ifdef __helper_serialize_h__forward__

# ifdef __helper__serialize_h__only_forward_declarations__
#  undef __helper__serialize_h__only_forward_declarations__
# else

// __helper_serialize_h__body__ is defined if function bodies were included already.
#  define __helper_serialize_h__body__

// headers that are needed for function bodies:
#  include <cmath>
#  ifdef _WIN32
#   include <winsock.h>
#   include <stdio.h>
#  else
#   include <arpa/inet.h>
#  endif

// SERIALIZATION OF BASIC TYPES

// int
inline					std::basic_string<int32_t> serialize(int a) // works for int, unsinged int, char and bool
{{{
	std::basic_string<int32_t> ret;
	ret += htonl(a);
	return ret;
}}}
inline                                  std::basic_string<int32_t> serialize(double & a) // has to be non-const reference, otherwise it is ambiguous with (int). NOTE that serialize/deserialize is not perfect for double. value may change slightly!
{{{
	std::basic_string<int32_t> ret;

	int exponent;
	double significand;
	int sig2;

	significand = std::frexp(a, &exponent);

	sig2 = significand * (2 << 30);

	ret += serialize(exponent);
	ret += serialize(sig2);

	return ret;
}}}
inline					bool deserialize(int & into, serial_stretch & serial)
{{{
	if(serial.empty()) return false;
	into = ntohl(*serial);
	serial.current++;
	return true;
}}}
inline					bool deserialize(unsigned int & into, serial_stretch & serial)
{{{
	if(serial.empty()) return false;
	into = ntohl(*serial);
	serial.current++;
	return true;
}}}
inline					bool deserialize(bool & into, serial_stretch & serial)
{{{
	if(serial.empty()) return false;
	into = ntohl(*serial);
	serial.current++;
	return true;
}}}
inline					bool deserialize(char & into, serial_stretch & serial)
{{{
	if(serial.empty()) return false;
	into = ntohl(*serial);
	serial.current++;
	return true;
}}}
inline					bool deserialize(double & into, serial_stretch & serial)
{{{
	int exponent;
	double significand;
	int sig2;

	if(!deserialize(exponent, serial)) return false;
	if(!deserialize(sig2, serial)) return false;

	significand = ((double)sig2) / (2 << 30);

	into = ldexp(significand, exponent);
	return true;
}}}


// SERIALIZATION OF COMPOUND TYPES


// pair<S,T>
template <typename S, typename T>	std::basic_string<int32_t> serialize(const std::pair<S, T> & p)
{{{
	std::basic_string<int32_t> ret;
	ret += serialize(p.first);
	ret += serialize(p.second);
	return ret;
}}}
template <typename S, typename T>	bool deserialize(std::pair<S, T> & p, serial_stretch & serial)
{{{
	if( ! deserialize(p.first, serial) ) return false;
	return deserialize(p.second, serial);
}}}


// list<S>
template <typename S>			std::basic_string<int32_t> serialize(const std::list<S> & l)
{{{
	std::basic_string<int32_t> serialized_list;
	typename std::list<S>::const_iterator li;
	int size = 0;

	for(li = l.begin(); li != l.end(); ++li) {
		size++;
		serialized_list += serialize(*li);
	}

	return serialize(size) + serialized_list;
}}}
template <typename S>			bool deserialize(std::list<S> & l, serial_stretch & serial)
{{{
	int size;
	S tmp;

	l.clear();

	if(!deserialize(size, serial)) return false;
	while(size) {
		if(!deserialize(tmp, serial)) return false;
		l.push_back(tmp);
		--size;
	}
	return true;
}}}


// vector<S>
template <typename S>			std::basic_string<int32_t> serialize(const std::vector<S> & v)
{{{
	std::basic_string<int32_t> ret;
	typename std::vector<S>::const_iterator li;

	ret += serialize(v.size());
	for(li = v.begin(); li != v.end(); ++li)
		ret += serialize(*li);

	return ret;
}}}
template <typename S>			bool deserialize(std::vector<S> & v, serial_stretch & serial)
{{{
	int size;
	S tmp;

	v.clear();

	if(!deserialize(size, serial)) return false;
	v.resize(size);
	for(int i = 0; i < size; ++i) {
		if(!deserialize(tmp, serial))
			return false;
		v[i] = tmp;
	}
	return true;
}}}


// set<S>
template <typename S>			std::basic_string<int32_t> serialize(const std::set<S> & s)
{{{
	std::basic_string<int32_t> ret;
	typename std::set<S>::const_iterator si;

	ret += serialize(s.size());
	for(si = s.begin(); si != s.end(); ++si)
		ret += serialize(*si);

	return ret;
}}}
template <typename S>			bool deserialize(std::set<S> & s, serial_stretch & serial)
{{{
	int size;
	S tmp;

	s.clear();

	if(!deserialize(size, serial)) return false;
	while(size) {
		if(!deserialize(tmp, serial)) return false;
		s.insert(tmp);
		--size;
	}
	return true;
}}}


// map<S, T>
template <typename S, typename T>	std::basic_string<int32_t> serialize(const std::map<S, T> & m)
{{{
	std::basic_string<int32_t> ret;
	typename std::map<S, T>::const_iterator mi;

	ret += serialize(m.size());
	for(mi = m.begin(); mi != m.end(); ++mi)
		ret += serialize(*mi);

	return ret;
}}}
template <typename S, typename T>	bool deserialize(std::map<S, T> & m, serial_stretch & serial)
{{{
	int size;
	std::pair<S, T> tmp;

	m.clear();

	if(!deserialize(size, serial)) return false;
	while(size) {
		if(!deserialize(tmp, serial)) return false;
		m.insert(tmp);
		size--;
	}
	return true;
}}}


// multimap<S, T>
template <typename S, typename T>	std::basic_string<int32_t> serialize(const std::multimap<S, T> & m)
{{{
	std::basic_string<int32_t> ret;
	typename std::multimap<S, T>::const_iterator mi;

	ret += serialize(m.size());
	for(mi = m.begin(); mi != m.end(); ++mi)
		ret += serialize(*mi);

	return ret;
}}}
template <typename S, typename T>	bool deserialize(std::multimap<S, T> & m, serial_stretch & serial)
{{{
	int size;
	std::pair<S, T> tmp;

	m.clear();

	if(!deserialize(size, serial)) return false;
	while(size) {
		if(!deserialize(tmp, serial)) return false;
		m.insert(tmp);
		size--;
	}
	return true;
}}}

// basic_string<S>
template <typename S>			std::basic_string<int32_t> serialize(const std::basic_string<S> & s)
{{{
	std::basic_string<int32_t> ret;
	typename std::basic_string<S>::const_iterator si;

	ret += serialize(s.length());
	for(si = s.begin(); si != s.end(); ++si)
		ret += serialize(*si);

	return ret;
}}}

template <typename S>			bool deserialize(std::basic_string<S> & s, serial_stretch & serial)
{{{
	int size;
	S tmp;

	s.clear();

	if(!deserialize(size, serial)) return false;
	while(size) {
		if(!deserialize(tmp, serial)) return false;
		s += tmp;
		size--;
	}
	return true;
}}}

# endif // else of ifdef __helper__serialize_h__only_forward_declarations__

#endif // IFNDEF __helper_serialize_h__body__

