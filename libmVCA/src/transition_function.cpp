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
	basic_string<int32_t> ret;

	ret += 0; // size, filled in later.

	// transitions are of type map<int, map<int, int> >, i.e. state -> sigma -> state
	// but we will serialize to (state, sigma, set<states>), s.t. the format is
	// compatible with nondeterministic_transition_function.
	map<int, map<int, int> >::iterator state_iterator;
	map<int, int>::iterator sigma_iterator;

	for(state_iterator = transitions.begin(); state_iterator != transitions.end(); ++state_iterator) {
		for(sigma_iterator = state_iterator->second.begin(); sigma_iterator != state_iterator->second.end(); ++sigma_iterator) {
			ret += htonl(state_iterator->first); // src
			ret += htonl(sigma_iterator->first); // label
			ret += htonl(1); // size of dst-set
			ret += htonl(sigma_iterator->second); // singleton-set of dst
		}
	}

	ret[0] = htonl(ret.length()-1);

	return ret;
}}}
bool deterministic_transition_function::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit, int & progress)
{
	
}
bool deterministic_transition_function::is_deterministic()
{ return true; };

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
	set<int>::iterator si;
	set<int> dst;

	set_insert(dst, this->transitions[*si][sigma]);

	return dst;
}}}

basic_string<int32_t> nondeterministic_transition_function::serialize()
{{{
	basic_string<int32_t> ret;

	ret += 0; // size, filled in later.

	map<int, map<int, set<int> > >::iterator state_iterator;
	map<int, set<int> >::iterator sigma_iterator;

	for(state_iterator = transitions.begin(); state_iterator != transitions.end(); ++state_iterator) {
		for(sigma_iterator = state_iterator->second.begin(); sigma_iterator != state_iterator->second.end(); ++sigma_iterator) {
			ret += htonl(state_iterator->first); // src
			ret += htonl(sigma_iterator->first); // label
			ret += serialize_integer_set(sigma_iterator->second); // dst
		}
	}

	ret[0] = htonl(ret.length()-1);

	return ret;
}}}
bool nondeterministic_transition_function::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit, int & progress)
{
	
}
bool nondeterministic_transition_function::is_deterministic()
{ return false; }; // FIXME: check on the fly

} // end of namespace libmVCA

