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

#include <libmVCA/transition_function.h>

#include "set.h"

namespace libmVCA {

using namespace std;

// INTERFACE

bool transition_function::test_and_transmute(int &m, set<int> &states, int sigma)
{{{
	enum pushdown_direction dm;

	if(m < 0 || !alphabet)
		goto bad_transmute;

	dm = alphabet->get_direction(sigma);

	if(dm == dir_indefinite)
		goto bad_transmute;

	m += (int)dm;
	if(m < 0)
		goto bad_transmute;

	return this->single_transmute(states, sigma);

bad_transmute:
	m = -1;
	states.clear();
	return false;
}}}
pair<int, set<int> > transition_function::transmute(int m, set<int> &states, int sigma, bool ok)
{{{
	pair<int, set<int> > ret;
	ret.first = m;
	ret.second = states;
	ok = this->test_and_transmute(ret.first, ret.second, sigma);
	return ret;
}}}
pair<int, set<int> > transition_function::transmute(int m, set<int> &states, int sigma)
{{{
	pair<int, set<int> > ret;
	ret.first = m;
	ret.second = states;
	this->test_and_transmute(ret.first, ret.second, sigma);
	return ret;
}}}
pair<int, set<int> > transition_function::transmute(int m, int state, int sigma, bool ok)
{{{
	pair<int, set<int> > ret;
	ret.first = m;
	ret.second.insert(state);
	ok = this->test_and_transmute(ret.first, ret.second, sigma);
	return ret;
}}}
pair<int, set<int> > transition_function::transmute(int m, int state, int sigma)
{{{
	pair<int, set<int> > ret;
	ret.first = m;
	ret.second.insert(state);
	this->test_and_transmute(ret.first, ret.second, sigma);
	return ret;
}}}

// DETERMINISTIC

basic_string<int32_t> deterministic_transition_function::serialize()
{
	
}
bool deterministic_transition_function::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	
}
bool deterministic_transition_function::is_deterministic()
{ return true; };
bool deterministic_transition_function::single_transmute(set<int> & states, int & sigma)
{{{
	set<int>::iterator si;

	set<int> s;

	for(si = states.begin(); si != states.end(); ++si)
		s.insert(this->transitions[*si][sigma]);

	states = s;
	return true;
}}}

// NONDETERMINISTIC

basic_string<int32_t> nondeterministic_transition_function::serialize()
{
	
}
bool nondeterministic_transition_function::deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
{
	
}
bool nondeterministic_transition_function::is_deterministic()
{ return false; }; // FIXME: check on the fly
bool nondeterministic_transition_function::single_transmute(set<int> & states, int & sigma)
{{{
	set<int>::iterator si;

	set<int> s;

	for(si = states.begin(); si != states.end(); ++si)
		set_insert(s, this->transitions[*si][sigma]);

	states = s;
	return true;
}}}

} // end of namespace libmVCA

