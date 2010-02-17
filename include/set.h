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
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __helper_set_h__
# define __helper_set_h__

# include <set>
# include <ostream>

// the set_*-functions are very generic and may be used with any kind of set

template <class T>	bool set_includes(const std::set<T> &superset, const std::set<T> &subset)
{{{
	typename std::set<T>::iterator Si, si;

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
{{{
	typename std::set<T>::iterator si;

	for(si = t.begin(); si != t.end(); ++si)
		s.insert(*si);

	return s;
}}}

template <class T>	void set_insert(std::set<T> &into, const std::set<T> &subset)
{{{
	typename std::set<T>::iterator si;

	for(si = subset.begin(); si != subset.end(); ++si)
		into.insert(*si);
}}}

template <class T>	std::set<T> set_intersect(const std::set<T> &s, const std::set<T> &t)
{{{
	std::set<T> ret;

	typename std::set<T>::iterator si;

	for(si = s.begin(); si != s.end(); ++si) {
		if(t.find(*si) != t.end())
			ret.insert(*si);
		// set::size() is O(1)
		if(ret.size() >= s.size() || ret.size() >= t.size())
			break;
	}

	return ret;
}}}

template <class T>	std::set<T> set_without(const std::set<T> &s, const std::set<T> &remove)
{{{
	std::set<T> ret;
	typename std::set<T>::iterator si, rmi;

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
	typename std::set<T>::iterator si;

	con << "{ ";
	si = s.begin();
	while(si != s.end()) {
		con << *si;
		si++;
		if(si != s.end())
			con << ", ";
	}
	con << " }";
}}}

#endif // __helper_set_h__

