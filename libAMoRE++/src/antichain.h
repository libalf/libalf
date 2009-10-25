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


// some templates for antichain-algorithms

#include <set>
#include <map>

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

template <class T>   set<T> set_subtract(set<T> &s, set<T> &remove)
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





template <class S, class T>   bool inner_set_includes(multimap< S, set<T> > &superset, multimap< S, set<T> > &subset)
{{{
	typename set< pair< S, set<T> > >::iterator si, start, end;

	for(si = subset.begin(); si != subset.end(); si++) {
		start = superset.lower_bound(si->first);
		end = superset.upper_bound(si->first);
		// check if element is contained in antichain
		while(start != end) {
			// set::size() is O(1)
			if(start->second.size() == si->second.size())
				if(set_includes(start->second, si->second))
					break;
			start++;
		}
		if(start == end)
			return false; // element is missing in superset
	}

	return true; // all elements contained
}}}

template <class S, class T>   multimap< S, set<T> > inner_set_union(multimap< S, set<T> > s, multimap< S, set<T> > &t)
// NOTE: this also calls inner_powerset_to_inclusion_antichain() !
{{{
	typename multimap< S, set<T> >::iterator ti, start, end;

	for(ti = t.begin(); ti != t.end(); ti++) {
		start = s.lower_bound(ti->first);
		end = s.upper_bound(ti->first);
		// check if element or superset of it is already in antichain
		while(start != end) {
			if(set_includes(start->second, ti->second)) {
				start = end;
				break;
			}
			start++;
		}
		// otherwise add it
		if(start == end)
			s.insert(*ti);
	}

	inner_powerset_to_inclusion_antichain(s); // FIXME: do we need this here? we check for inclusion, not equality above.
	return s;
}}}

template <class S, class T>   multimap< S, set<T> > inner_set_intersect(multimap< S, set<T> > &s, multimap< S, set<T> > &t)
// NOTE: this also calls inner_powerset_to_inclusion_antichain() !
{{{
	multimap< S, set<T> > ret;

	typename multimap< S, set<T> >::iterator ti, start, end;

	for(ti = t.begin(); ti != t.end(); ti++) {
		start = s.lower_bound(ti->first);
		end = s.upper_bound(ti->first);

		pair< S, set<T> > q;
		q.first = ti->first;

		while(start != end) {
			q.second = set_intersect(ti->second, start->second);
			if(!q.second.empty())
				ret.insert(q);
		}
	}

	inner_powerset_to_inclusion_antichain(ret);
	return ret;
}}}

template <class S, class T>   void inner_powerset_to_inclusion_antichain(multimap< S, set<T> > &antichain)
// FIXME: this is rather inefficient
{{{
	typename multimap< S, set<T> >::iterator si, start, end;
	list< pair< S, set<T> > > superfluous;
	typename list< pair< S, set<T> > >::iterator sui;

	// find superfluous elements
	for(si = antichain.begin(); si != antichain.end(); ++si){
		start = antichain.lower_bound(si->first);
		end = antichain.upper_bound(si->first);
		// check if there is a superset
		while(start != end) {
			if(start != si)
				if(set_includes(start->second, si->second))
					break;
			start++;
		}
		if(start != end)
			superfluous.push_back(*si);
	}

	// remove superfluous elements
	for(sui = superfluous.begin(); sui != superfluous.end(); ++sui) {
		start = antichain.lower_bound(si->first);
		end = antichain.upper_bound(si->first);
		while(start != end) {
			if(start->second.size() == si->second.size())
				if(start->second == si->second)
					break;
			start++;
		}
		antichain.erase(start);
	}
}}}

