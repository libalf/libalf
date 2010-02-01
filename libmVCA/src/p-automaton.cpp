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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <libmVCA/p-automaton.h>

#include <list>
#include <set>

namespace libmVCA {

using namespace std;



bool operator<(const pa_transition_target first, const pa_transition_target second)
{ return (first.dst < second.dst); }

bool operator==(const pa_transition_target first, const pa_transition_target second)
{ return (first.dst == second.dst); }

bool operator>(const pa_transition_target first, const pa_transition_target second)
{ return (first.dst > second.dst); }



p_automaton::p_automaton()
{{{
	clear();
}}}
p_automaton::p_automaton(mVCA * base_automaton)
{{{
	clear();
	initialize(base_automaton);
}}}
void p_automaton::clear()
{{{
	valid = false;
	saturated = false;
	base_automaton = NULL;
	state_count = 0;
	alphabet_size = 0;
	initial.clear();
	final.clear();
	transitions.clear();
}}}

bool p_automaton::initialize(mVCA * base_automaton)
{{{
	clear();

	this->base_automaton = base_automaton;
	this->state_count = this->base_automaton->get_state_count();
	this->alphabet_size = this->base_automaton->get_alphabet_size();

	for(int i = 0; i < this->state_count; ++i)
		this->initial.insert(i);

	valid = true;
}}}

bool p_automaton::add_accepting_configuration(int state, int m)
{{{
	if(!valid)
		return false;
	saturated = false;

	if(m == 0) {
		final.insert(state);
		return true;
	}

	// allocate transitional states
	list<int> tr;
	for(int i = 1; i < m; ++i) {
		tr.push_back(state_count);
		state_count++;
	}

	pa_transition_target target;
	target.is_new = false;

	// add transitions
	while(!tr.empty()) {
		int next;
		next = tr.front();
		tr.pop_front();

		target.dst = next;
		transitions[state].insert(target);
		state = next;
	}

	// allocate final state
	int f = state_count;
	state_count++;
	final.insert(f);

	// add transition to final state
	target.dst = f;
	transitions[state].insert(target);

	return true;
}}}

bool p_automaton::saturate_preSTAR()
{
	if(!valid || saturated)
		return false;

	
}

list<int> p_automaton::check_acceptance(int state, int m, bool & reachable)
{
	list<int> ret;

	if(!valid) {
		reachable = false;
		return ret;
	}

	
}

}; // end  of namespace libmVCA

