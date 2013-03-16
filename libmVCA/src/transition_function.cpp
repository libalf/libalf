/* vim: fdm=syntax foldlevel=1 foldnestmax=2
 * $Id$
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
 *           and David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifdef _WIN32
# include <winsock.h>
#else
# include <arpa/inet.h>
#endif

#include <stdio.h>

#include <libmVCA/transition_function.h>

#include <libmVCA/serialize.h>

namespace libmVCA {

using namespace std;

// INTERFACE

void transition_function::endo_transmute(set<int> & states,int sigma) const
{
	set<int> s;
	s = this->transmute(states, sigma);
	states.swap(s); // swap is O(1)
}

// DETERMINISTIC

set<int> deterministic_transition_function::transmute(const set<int> & states, int sigma) const
{
	set<int>::const_iterator si;
	set<int> dst;
	map<int, map<int, int> >::const_iterator statei;
	map<int, int>::const_iterator labeli;

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
}
set<int> deterministic_transition_function::transmute(int state, int sigma) const
{
	set<int> dst;
	map<int, map<int, int> >::const_iterator statei;
	map<int, int>::const_iterator labeli;

	statei = transitions.find(state);
	if(statei != transitions.end()) {
		labeli = statei->second.find(sigma);
		if(labeli != statei->second.end()) {
			dst.insert(labeli->second);
		}
	}
	//dst.insert(this->transitions[state][sigma]);

	return dst;
}

basic_string<int32_t> deterministic_transition_function::serialize() const
{
	return ::serialize(transitions);
}
bool deterministic_transition_function::deserialize(::serial_stretch serial)
{
	return ::deserialize(transitions, serial);
}
bool deterministic_transition_function::is_deterministic() const
{ return true; };
string deterministic_transition_function::get_transition_dotfile(int m, int m_bound) const
{
	string ret;
	char buf[128];
	map<int, map<int, int> >::const_iterator mmi;
	map<int, int>::const_iterator mi;

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
}

// NONDETERMINISTIC

set<int> nondeterministic_transition_function::transmute(const set<int> & states, int sigma) const
{
	set<int>::const_iterator si;
	set<int> dst;

	for(si = states.begin(); si != states.end(); ++si) {
		const_iterator i;
		i = transitions.find(*si);
		if(i != transitions.end()) {
			map<int, set<int> >::const_iterator msi;
			msi = i->second.find(sigma);
			if(msi != i->second.end())
				dst.insert(msi->second.begin(), msi->second.end());
		}
	}

	return dst;
}
set<int> nondeterministic_transition_function::transmute(int state, int sigma) const
{
	set<int> dst;

	const_iterator i;
	i = transitions.find(state);
	if(i != transitions.end()) {
		map<int, set<int> >::const_iterator msi;
		msi = i->second.find(sigma);
		if(msi != i->second.end())
			dst.insert(msi->second.begin(), msi->second.end());
	}

	return dst;
}

basic_string<int32_t> nondeterministic_transition_function::serialize() const
{
	return ::serialize(transitions);
}
bool nondeterministic_transition_function::deserialize(::serial_stretch serial)
{
	return ::deserialize(transitions, serial);
}
bool nondeterministic_transition_function::is_deterministic() const
{ return false; }; // TODO: check on the fly
string nondeterministic_transition_function::get_transition_dotfile(int m, int m_bound) const
{
	string ret;
	char buf[128];
	map<int, map<int, set<int> > >::const_iterator mmsi;
	map<int, set<int> >::const_iterator msi;
	set<int>::const_iterator si;

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
}

} // end of namespace libmVCA

