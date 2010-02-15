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
	mVCA_premap.clear();
	mVCA_postmap.clear();
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

	this->base_automaton->get_transition_maps(mVCA_premap, mVCA_postmap);

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
	while(m >= base_automaton->get_m_bound()) {
		transition.dst = new_state();
		transitions[state][ base_automaton->get_m_bound() ].insert( transition );
		state = transition.dst;
		--m;
	}

	// add intermediate transition for m-values below m_bound
	if(m > 0) {
		transition.dst = new_state();
		transitions[state][m].insert( transition );
		state = transition.dst;
	}

	// allocate final state and add transition to it.
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

	// using the transition rules in mVCA_postmap, we saturate the automaton:
	//
	// for all given mVCA-rules  (from_state, m, mVCA-label) -> (to_state, new_m) , the change in the PDS-config looks like this:
	// <from_state, m*> -> <to_state, new_m*>, where m* and new_m* are valid PDS representations of the counter m resp. new_m.
	//
	// for each of these rules we do the following:
	//
	// we calculate states reachable via <to_state, new_m*> and remember the possible mVCA-transitions T required for the configuration.
	//
	// then we add a transition from <from_state> with label "top-of-<m*>" and
	// set the mVCA-transition of this transition to concat( T, mVCA-label ). (FIXME: or concat( mVCA-label, T )? )

	bool new_transition_added = true;

	while(new_transition_added) {
		new_transition_added = false;
		// FIXME: iterate over all mVCA transition rules:
		int from_state, from_m, to_state, to_m, mVCA_label;
		
		{
			// assume a rule <from_state, from_m> -> <to_state, to_m> with mVCA-label <mVCA_label>
			list<int> to_m_cfg = get_config(to_state, to_m);
			to_m_cfg.pop_front();
			if(from_m != 0)
				to_m_cfg.pop_back(); // we have to keep the bottom symbol if from_m == 0.

			set<pair<int, list<int> > > destinations;
			set<pair<int, list<int> > >::iterator di;

			destinations = run_transition_accumulate(to_state, to_m_cfg);

			for(di = destinations.begin(); di != destinations.end(); ++di) {
				if(!transition_exists(from_state, from_m, to_state)) {
					pa_transition_target tr;

					tr.dst = to_state;
					tr.mVCA_word = di->second;
					tr.mVCA_word.push_back(mVCA_label); // FIXME: push_front(...)?

					transitions[from_state][from_m].insert( tr );
					new_transition_added = true;
				}
			}
		}
	}

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
bool p_automaton::transition_exists(int from_state, int label, int to_state)
{{{
	set<pa_transition_target>::iterator tri;
	for(tri = transitions[from_state][label].begin(); tri != transitions[from_state][label].end(); ++tri)
		if(tri->dst == to_state)
			return true;
	return false;
}}}
set<int> p_automaton::run_transition(int from_state, int label)
{{{
	set<int> ret;
	set<pa_transition_target>::iterator tri;
	for(tri = transitions[from_state][label].begin(); tri != transitions[from_state][label].end(); ++tri)
		ret.insert(tri->dst);
	return ret;
}}}

// local helper function for run_transition_accumulate:
static inline list<int> operator+(list<int> prefix, list<int> suffix)
{{{
	list<int> ret;
	ret = prefix;
	for(list<int>::iterator wi = suffix.begin(); wi != suffix.end(); ++wi)
		ret.push_back(*wi);
	return ret;
}}}

set< pair<int, list<int> > > p_automaton::run_transition_accumulate(int from_state, int label, list<int> current_mVCA_run)
{{{
	set< pair<int, list<int> > > ret;
	set<pa_transition_target>::iterator tri;

	for(tri = transitions[from_state][label].begin(); tri != transitions[from_state][label].end(); ++tri)
		ret.insert(  pair<int, list<int> >(tri->dst, current_mVCA_run + tri->mVCA_word)  );

	return ret;
}}}
set< pair<int, list<int> > > p_automaton::run_transition_accumulate(int from_state, list<int> word)
{{{
	list<int>::iterator wi;

	map<int, list<int> > current;
	map<int, list<int> > next;
	map<int, list<int> >::iterator ci, ni;

	set< pair<int, list<int> > > ret;
	set< pair<int, list<int> > >::iterator ri;


	current[from_state] = list<int>();

	for(wi = word.begin(); wi != word.end(); ++wi) {
		next.clear();
		for(ci = current.begin(); ci != current.end(); ++ci) {
			ret = run_transition_accumulate(ci->first, *wi, ci->second);
			// join with other new states
			for(ri = ret.begin(); ri != ret.end(); ++ri) {
				ni = next.find(ri->first);
				// pick the shorter one if there already exists a run to this state.
				if( ni == next.end() || ( ni->second.size() > ri->second.size() ) )
					next[ri->first] = ri->second;
			}
		}

		current.swap(next);
	}

	// transform to standard interface
	ret.clear();
	for(ci = current.begin(); ci != current.end(); ++ci)
		ret.insert(*ci);

	return ret;
}}}


}; // end  of namespace libmVCA

