/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <set>

namespace amore {

using namespace std;

// the set_*-functions are very generic and may be used for any kind of set

template <class T>   bool set_includes(set<T> &superset, set<T> &subset)
{{{
	typename set<T>::iterator Si, si;

	Si = superset.begin();
	si = subset.begin();

	// set::size() is O(1)
	if(subset.size() > superset.size())
		return false;

	while( Si != superset.end() && si != subset.end() ) {
		if(*si == *Si) {
			++si;
		} else {
			if(*si > *Si)
				return false;
			else
				++Si;
		}
	}

	return ( Si != superset.end() || si == subset.end() );
}}}

template <class T>   set<T> set_union(set<T> s, set<T> &t)
{{{
	typename set<T>::iterator si;

	for(si = t.begin(); si != t.end(); ++si)
		s.insert(*si);

	return s;
}}}

template <class T>   set<T> set_intersect(set<T> &s, set<T> &t)
{{{
	set<T> ret;

	typename set<T>::iterator si;

	for(si = s.begin(); si != s.end(); ++si) {
		if(t.find(*si) != t.end())
			ret.insert(*si);
		// set::size() is O(1)
		if(ret.size() >= s.size() || ret.size() >= t.size())
			break;
	}

	return ret;
}}}

template <class T>   void powerset_to_inclusion_antichain(set<set<T> > &antichain)
// FIXME: this is rather inefficient
{{{
	set<set<T> > included;
	typename set<set<T> >::iterator si1, si2;

	for(si1 = antichain.begin(); si1 != antichain.end(); ++si1) {
		set<T> s1 = *si1;
		for(si2 = si1, si2++; si2 != antichain.end(); ++si2) {
			set<T> s2 = *si2;

			if(set_includes(s2, s1)) {
				included.insert(s1);
				break; // all later subsets of s1 will also be subsets of s1
			} else {
				if(set_includes(s1, s2))
					included.insert(s2);
			}
		}
	}

	for(si1 = included.begin(); si1 != included.end(); ++si1)
		antichain.erase(*si1);
}}}

template <class T>   set<T> set_without(set<T> &s, set<T> &remove)
{{{
	set<T> ret;
	typename set<T>::iterator si, rmi;

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

} // end namespace amore

