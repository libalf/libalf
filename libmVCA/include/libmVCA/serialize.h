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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __helper_serialize_h__
# define __helper_serialize_h__

# include <string>
# include <list>
# include <set>
# include <map>
# include <iterator>
# include <vector>

# include <stdint.h>

# ifdef _WIN32
#  include <winsock.h>
#  include <stdio.h>
# else
#  include <arpa/inet.h>
# endif

class  serial_stretch {
	public:
		std::basic_string<int32_t>::iterator current;
		std::basic_string<int32_t>::iterator limit;
	public:
		inline bool empty()
		{ return (current == limit); };
		inline void operator++()
		{ current++; };
		inline int operator*()
		{ return *current; };
};

// forward declarations
inline                                  std::basic_string<int32_t> serialize(int a);
inline                                  bool deserialize(int & into, serial_stretch & serial);
inline                                  bool deserialize(unsigned int & into, serial_stretch & serial);
inline                                  bool deserialize(bool & into, serial_stretch & serial);
template <typename S, typename T>       std::basic_string<int32_t> serialize(std::pair<S, T> & p);
template <typename S, typename T>       bool deserialize(std::pair<S, T> & p, serial_stretch & serial);
template <typename S>                   std::basic_string<int32_t> serialize(std::list<S> & l);
template <typename S>                   bool deserialize(std::list<S> & l, serial_stretch & serial);
template <typename S>                   std::basic_string<int32_t> serialize(std::vector<S> & v);
template <typename S>                   bool deserialize(std::vector<S> & v, serial_stretch & serial);
template <typename S>                   std::basic_string<int32_t> serialize(std::set<S> & s);
template <typename S>                   bool deserialize(std::set<S> s, serial_stretch & serial);
template <typename S, typename T>       std::basic_string<int32_t> serialize(std::map<S, T> & m);
template <typename S, typename T>       bool deserialize(std::map<S, T> m, serial_stretch & serial);
template <typename S, typename T>	std::basic_string<int32_t> serialize(std::multimap<S, T> & m);
template <typename S, typename T>	bool deserialize(std::multimap<S, T> m, serial_stretch & serial);

// SERIALIZATION OF BASIC TYPES


// int
inline					std::basic_string<int32_t> serialize(int a) // works for int, unsinged int and bool.
{{{
	std::basic_string<int32_t> ret;
	ret += htonl(a);
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


// SERIALIZATION OF COMPOUND TYPES


// pair<S,T>
template <typename S, typename T>	std::basic_string<int32_t> serialize(std::pair<S, T> & p)
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
template <typename S>			std::basic_string<int32_t> serialize(std::list<S> & l)
{{{
	std::basic_string<int32_t> serialized_list;
	typename std::list<S>::iterator li;
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
template <typename S>			std::basic_string<int32_t> serialize(std::vector<S> & v)
{{{
	std::basic_string<int32_t> ret;
	typename std::vector<S>::iterator li;

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
	v.reserve(size);
	for(int i = 0; i < size; ++i) {
		if(!deserialize(tmp, serial))
			return false;
		v[i] = tmp;
	}
	return true;
}}}


// set<S>
template <typename S>			std::basic_string<int32_t> serialize(std::set<S> & s)
{{{
	std::basic_string<int32_t> ret;
	typename std::set<S>::iterator si;

	ret += serialize(s.size());
	for(si = s.begin(); si != s.end(); ++si)
		ret += serialize(*si);

	return ret;
}}}
template <typename S>			bool deserialize(std::set<S> s, serial_stretch & serial)
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
template <typename S, typename T>	std::basic_string<int32_t> serialize(std::map<S, T> & m)
{{{
	std::basic_string<int32_t> ret;
	typename std::map<S, T>::iterator mi;

	ret += serialize(m.size());
	for(mi = m.begin(); mi != m.end(); ++mi)
		ret += serialize(*mi);

	return ret;
}}}
template <typename S, typename T>	bool deserialize(std::map<S, T> m, serial_stretch & serial)
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
template <typename S, typename T>	std::basic_string<int32_t> serialize(std::multimap<S, T> & m)
{{{
	std::basic_string<int32_t> ret;
	typename std::multimap<S, T>::iterator mi;

	ret += serialize(m.size());
	for(mi = m.begin(); mi != m.end(); ++mi)
		ret += serialize(*mi);

	return ret;
}}}
template <typename S, typename T>	bool deserialize(std::multimap<S, T> m, serial_stretch & serial)
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

#endif // __helper_serialize_h__

