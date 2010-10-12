/* $Id$
 * vim: fdm=marker
 *
 * set<> helper functions
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
 * (c) 2008,2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * (c) 2010 David R. Piegdon
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __helper_set_h__
# define __helper_set_h__

# include <set>
# include <ostream>
# include <sstream>

// the set_*-functions are very generic and may be used with any kind of set

// set_includes uses the order of both sets for optimizations.
template <class T>	bool set_includes(const std::set<T> &superset, const std::set<T> &subset)
{{{
	typename std::set<T>::const_iterator Si, si;

	Si = superset.begin();
	si = subset.begin();

	// set::size() is O(1)
	if(subset.size() > superset.size())
		return false;

	while( Si != superset.end() && si != subset.end() ) {
		if(*si == *Si) {
			++si;
		} else {
			if(*si < *Si)
				return false;
			else
				++Si;
		}
	}

	return ( Si != superset.end() || si == subset.end() );
}}}

template <class T>	std::set<T> set_union(std::set<T> s, const std::set<T> &t)
{ s.insert(t.begin(), t.end()); return s; }

// set_insert(...):
// use set::insert(begin(), end()) instead.

// set_intersect uses the order of both sets for optimizations.
template <class T>	std::set<T> set_intersect(const std::set<T> &s, const std::set<T> &t)
{{{
	std::set<T> ret;

	typename std::set<T>::const_iterator si;

	for(si = s.begin(); si != s.end(); ++si) {
		if(t.find(*si) != t.end())
			ret.insert(*si);
		if(ret.size() >= s.size() || ret.size() >= t.size())
			break;
	}

	return ret;
}}}

// set_without uses the order of both sets for optimizations. thus it is more efficient than set::erase(...)
template <class T>	std::set<T> set_without(const std::set<T> &s, const std::set<T> &remove)
{{{
	std::set<T> ret;
	typename std::set<T>::const_iterator si, rmi;

	si = s.begin();
	rmi = remove.begin();

	while( si != s.end() && rmi != remove.end() ) {
		if(*si < *rmi) {
			ret.insert(*si);
			++si;
		} else {
			if(*si == *rmi) {
				++si;
				++rmi;
			} else {
				++rmi;
			}
		}
	}
	while(si != s.end()) {
		ret.insert(*si);
		++si;
	}

	return ret;
}}}

template <class T>	void print_set(std::ostream &con, const std::set<T> &s)
{{{
	std::set<int>::const_iterator si;

        con << "{ ";
        si = s.begin();
        if(si != s.end()) {
                con <<  *si;
                ++si;
                while(si != s.end()) {
                        con << ", " << *si;
                        ++si;
                };
        }
        con << " }";
}}};

template <class T>	std::string set2string(const std::set<T> &s)
{{{
	std::stringstream str;
	std::set<int>::const_iterator si;

	print_set(str, s);

	return str.str();
}}};

#endif // __helper_set_h__

