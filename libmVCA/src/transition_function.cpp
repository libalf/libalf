/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libmVCA.
 *
 * libmVCA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libmVCA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmVCA.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009,2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifdef _WIN32
# include <winsock.h>
# include <stdio.h>
#else
# include <arpa/inet.h>
#endif

#include <libmVCA/transition_function.h>

#include <libmVCA/serialize.h>

#include "set.h"

namespace libmVCA {

using namespace std;

// INTERFACE

void transition_function::endo_transmute(set<int> & states,int sigma)
{{{
	set<int> s;
	s = this->transmute(states, sigma);
	states.swap(s); // swap is O(1)
}}}

// DETERMINISTIC

set<int> deterministic_transition_function::transmute(const set<int> & states, int sigma)
{{{
	set<int>::iterator si;
	set<int> dst;
	map<int, map<int, int> >::iterator statei;
	map<int, int>::iterator labeli;

	for(si = states.begin(); si != states.end(); ++si) {
		statei = transitions.find(*si);
		if(statei != transitions.end()) {
			labeli = statei->second.find(sigma);
			if(labeli != statei->second.end()) {
				dst.insert(labeli->second);
			}
		}
		//dst.insert(this->transitions[*si][sigma]);
	}

	return dst;
}}}
set<int> deterministic_transition_function::transmute(int state, int sigma)
{{{
	set<int> dst;
	map<int, map<int, int> >::iterator statei;
	map<int, int>::iterator labeli;

	statei = transitions.find(state);
	if(statei != transitions.end()) {
		labeli = statei->second.find(sigma);
		if(labeli != statei->second.end()) {
			dst.insert(labeli->second);
		}
	}
	//dst.insert(this->transitions[state][sigma]);

	return dst;
}}}

basic_string<int32_t> deterministic_transition_function::serialize()
{{{
	return ::serialize(transitions);
}}}
bool deterministic_transition_function::deserialize(::serial_stretch serial)
{{{
	return ::deserialize(transitions, serial);
}}}
bool deterministic_transition_function::is_deterministic()
{ return true; };
string deterministic_transition_function::get_transition_dotfile(int m, int m_bound)
{{{
	string ret;
	char buf[128];
	map<int, map<int, int> >::iterator mmi;
	map<int, int>::iterator mi;

	for(mmi = transitions.begin(); mmi != transitions.end(); ++mmi) {
		for(mi = mmi->second.begin(); mi != mmi->second.end(); ++mi) {
			snprintf(buf, 128, "\tq%d -> q%d [ label = \"m%da%d\" %s];\n",
					mmi->first, mi->second, m, mi->first,
					m == m_bound ? ", color=\"red\" " : ""
				);
			ret += buf;
		}
	}
	return ret;
}}}

// NONDETERMINISTIC

set<int> nondeterministic_transition_function::transmute(const set<int> & states, int sigma)
{{{
	set<int>::iterator si;
	set<int> dst;

	for(si = states.begin(); si != states.end(); ++si)
		set_insert(dst, this->transitions[*si][sigma]);

	return dst;
}}}
set<int> nondeterministic_transition_function::transmute(int state, int sigma)
{{{
	set<int> dst;

	set_insert(dst, this->transitions[state][sigma]);

	return dst;
}}}

basic_string<int32_t> nondeterministic_transition_function::serialize()
{{{
	return ::serialize(transitions);
}}}
bool nondeterministic_transition_function::deserialize(::serial_stretch serial)
{{{
	return ::deserialize(transitions, serial);
}}}
bool nondeterministic_transition_function::is_deterministic()
{ return false; }; // FIXME: check on the fly
string nondeterministic_transition_function::get_transition_dotfile(int m, int m_bound)
{{{
	string ret;
	char buf[128];
	map<int, map<int, set<int> > >::iterator mmsi;
	map<int, set<int> >::iterator msi;
	set<int>::iterator si;

	for(mmsi = transitions.begin(); mmsi != transitions.end(); ++mmsi) {
		for(msi = mmsi->second.begin(); msi != mmsi->second.end(); ++msi) {
			for(si = msi->second.begin(); si != msi->second.end(); ++si) {
				snprintf(buf, 128, "\tq%d -> q%d [ label = \"m%da%d\" %s];\n",
						mmsi->first, *si, m, msi->first,
						m == m_bound ? ", color=\"red\" " : ""
					);
				ret += buf;
			}
		}
	}
	return ret;
}}}

} // end of namespace libmVCA

