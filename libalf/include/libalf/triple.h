/* $Id$
 * vim: fdm=marker
 *
 * triple<>, like a simple pair<> with three elements.
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

#ifndef __helper_triple_h__
# define __helper_triple_h__

#include "serialize.h"

template<class _T1, class _T2, class _T3>
class triple {
	public:
		typedef _T1 first_type;
		typedef _T2 second_type;
		typedef _T3 third_type;

		_T1 first;
		_T2 second;
		_T3 third;

		triple() : first(), second(), third() { }

		triple(const _T1& a, const _T2& b, const _T3& c) : first(a), second(b), third(c) { }

		void swap(triple & o)
		{ swap(first, o.swap); swap(second, o.second); swap(third, o.third); }

		triple & operator=(const triple & o)
		{ first = o.first; second = o.second; third = o.third; return *this; }

		bool operator==(const triple & o)
		{ return (first == o.first) && (second == o.second) && (third == o.third); }

		bool operator!=(const triple & o)
		{ return (first != o.first) && (second != o.second) && (third != o.third); }

		bool operator<(const triple & o)
		{ return first < o.first || (first == o.first && ( second < o.second || ( second == o.second && third < o.third ) ) ); }

		bool operator<=(const triple & o)
		{ return *this == o || *this < o; };

		bool operator>(const triple & o)
		{ return first > o.first || (first == o.first && ( second > o.second || ( second == o.second && third > o.third ) ) ); }

		bool operator>=(const triple & o)
		{ return *this == o || *this > o; };
};

// triple<R,S,T>
template <typename S, typename T, typename R> std::basic_string<int32_t> serialize(triple<R, S, T> & t)
{{{
	std::basic_string<int32_t> ret;
	ret += serialize(t.first);
	ret += serialize(t.second);
	ret += serialize(t.third);
	return ret;
}}}
template <typename S, typename T, typename R> bool deserialize(triple<R, S, T> & t, serial_stretch & serial)
{{{
	if( ! deserialize(t.first, serial) ) return false;
	if( ! deserialize(t.second, serial) ) return false;
	return deserialize(t.third, serial);
}}}

#endif // __helper_triple_h__

