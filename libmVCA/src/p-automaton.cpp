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
	alphabet_size = 0; // this is NOT the alphabet size of the mVCA, but the alphabet size of a PDS modeling the mVCA.
	highest_initial_state = -1;
	final.clear();
	transitions.clear();
}}}

bool p_automaton::initialize(mVCA * base_automaton)
{{{
	clear();

	if(NULL == base_automaton)
		return false;

	this->base_automaton = base_automaton;

	state_count = this->base_automaton->get_state_count();
	alphabet_size = this->base_automaton->get_m_bound() + 1; // we've got a completely different alphabet here!
	highest_initial_state = state_count - 1;

	valid = true;
	saturated = false;

	return true;
}}}

bool p_automaton::add_accepting_configuration(int state, int m)
{{{
	if(!valid)
		return false;

	pa_transition_target transition;

	// add transitions for states if m is >= m_bound:
	transition.labels.push_back( base_automaton->get_m_bound() );
	while(m >= base_automaton->get_m_bound()) {
		transition.dst = new_state();
		transitions[state][ base_automaton->get_m_bound() ].insert( transition );
		state = transition.dst;
		--m;
	}

	// add intermediate transition for m-values below m_bound
	if(m > 0) {
		transition.labels.clear();
		transition.labels.push_back(m);
		transition.dst = new_state();
		transitions[state][m].insert( transition );
		state = transition.dst;
	}

	// allocate final state and add transition to it.
	transition.labels.clear();
	transition.labels.push_back(0); // stack-empty-symbol for last transition
	transition.dst = new_state();
	final.insert(transition.dst);
	transitions[state][0].insert( transition );


	saturated = false;

	return true;
}}}

bool p_automaton::saturate_preSTAR()
{
	if(!valid)
		return false;

	if(saturated || final.empty())
		return true;

	
}

list<int> p_automaton::get_valid_run(int state, int m, bool & reachable)
{
	list<int> ret;

	if(!valid || final.empty()) {
		reachable = false;
		return ret;
	}

	

	// if not reachable, return empty word.
	
}

list<int> p_automaton::get_shortest_valid_run(int state, int m, bool & reachable)
{
	list<int> ret;

	if(!valid || final.empty()) {
		reachable = false;
		return ret;
	}

	// make sure we get the shortest run
	

	// if not reachable, return empty word.
	
}






int p_automaton::new_state()
{{{
	int n = state_count;
	++state_count;
	return n;
}}}
list<int> p_automaton::get_config(int state, int m)
{{{
	list<int> config;

	if(state>=state_count)
		return config; // invalid config.
	if(m < 0)
		return config; // invalid config.

	config.push_back(state);

	while(m >= base_automaton->get_m_bound()) {
		config.push_back(base_automaton->get_m_bound());
		--m;
	}

	if(m > 0)
		config.push_back(m);

	config.push_back(0); // stack-bottom-symbol

	return config;
}}}


}; // end  of namespace libmVCA

