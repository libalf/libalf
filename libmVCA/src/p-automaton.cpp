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
 * (c) 2010 David R. Piegdon <david-i2@piegdon.de>
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#include <list>
#include <set>

#include <stdio.h>

#include <iostream>

#include <libmVCA/p-automaton.h>

namespace libmVCA {

using namespace std;



// local helper functions
static inline list<int> operator+(list<int> prefix, list<int> suffix)
{
	list<int> ret;
	ret = prefix;
	for(list<int>::iterator wi = suffix.begin(); wi != suffix.end(); ++wi)
		ret.push_back(*wi);
	return ret;
}
static inline string word2string(list<int> word, char separator = '.')
{
	string ret;
	char buf[32];

	ret += separator;

	for(list<int>::iterator wi = word.begin(); wi != word.end(); wi++) {
		snprintf(buf, 32, "%d%c", *wi, separator);
		buf[31] = 0;
		ret += buf;
	}

	return ret;
}



bool operator<(const pa_transition_target first, const pa_transition_target second)
{ return (first.dst < second.dst); }

bool operator==(const pa_transition_target first, const pa_transition_target second)
{ return (first.dst == second.dst); }

bool operator>(const pa_transition_target first, const pa_transition_target second)
{ return (first.dst > second.dst); }



p_automaton::p_automaton()
{
	clear();
}
p_automaton::p_automaton(const mVCA * base_automaton)
{
	clear();
	initialize(base_automaton);
}
void p_automaton::clear()
{
	valid = false;
	saturated = false;
	added_configurations.clear();
	base_automaton = NULL;
	mVCA_postmap.clear();
	state_count = 0;
	alphabet_size = 0; // this is NOT the alphabet size of the mVCA, but the alphabet size of a PDS modeling the mVCA.
	highest_initial_state = -1;
	final.clear();
	transitions.clear();
}

bool p_automaton::initialize(const mVCA * base_automaton)
{
	clear();

	if(NULL == base_automaton)
		return false;

	this->base_automaton = base_automaton;

	state_count = this->base_automaton->get_state_count();
	alphabet_size = this->base_automaton->get_m_bound() + 1; // we've got a completely different alphabet here!
	highest_initial_state = state_count - 1;

	this->base_automaton->get_transition_map(mVCA_postmap);

	valid = true;
	saturated = false;

	return true;
}

bool p_automaton::add_accepting_configuration(int state, int m)
{
	if(!valid)
		return false;

	pair<int, int> p;
	p.first = state;
	p.second = m;
	if(added_configurations.find(p) != added_configurations.end())
		return true; // was already added.
	else
		added_configurations.insert(p);

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
}

static inline list<int> get_next_stack_top(int topmost_symbol, int m_bound, enum pushdown_direction dir)
// given the topmost PDS config symbol from_m and a direction, calculate the successor configuration.
{
	list<int> ret;

	int next_m;

	next_m = topmost_symbol + dir;

	ret.push_back(next_m);

	// special cases:
	//
	// OLD M	NEW M		   NEW STACK TOP
	// 0		0		-> 0						normal case
	// 0		1		-> 1,0
	// 1		0		-> ∅
	// m_bound-1	m_bound		-> m_bound, m_bound-1
	// m_bound	m_bound		-> m_bound					normal case
	// m_bound	m_bound-1	-> ∅
	// m_bound	m_bound+1	-> m_bound, m_bound

	if(topmost_symbol == 0 && next_m == 1) {
		ret.push_back(0);
	} else if(topmost_symbol == 1 && next_m == 0) {
		ret.pop_back();
	} else if(topmost_symbol == m_bound-1 && next_m == m_bound) {
		ret.push_back(m_bound-1);
	} else if(topmost_symbol == m_bound) {
		if(next_m == m_bound-1) {
			ret.pop_front();
		} else if(next_m == m_bound+1) {
			ret.push_back(m_bound);
		}
	}

	return ret;
}

bool p_automaton::saturate_preSTAR()
{
	if(!valid)
		return false;

	if(saturated || final.empty())
		return true;

	// using the transition rules in mVCA_postmap, we saturate the automaton:
	//
	// for all given mVCA-rules  (from_state, from_m, mVCA-label) -> (to_state, new_m) , the change in the PDS-config looks like this:
	// <from_state, from_m*> -> <to_state, new_m*>, where m* and new_m* are valid PDS representations of the counter m resp. new_m.
	//
	// for each of these rules we do the following:
	//
	// we calculate states reachable via <to_state, new_m*> and remember the possible mVCA-transitions T required for the configuration.
	//
	// then we add a transition from <from_state> with label "top-of-<m*>" and
	// set the mVCA-transition of this transition to concat( mVCA-label, T ).

	bool new_transition_added = true;

//	int run = 0;

	while(new_transition_added) {
//		cout << "\nrun: " << run << "\n";
//		run++;
		new_transition_added = false;
		// iterate over all mVCA transition rules:
		map<int, map<int, map<int, set<int> > > >::iterator mi; // over all m
		for(mi = mVCA_postmap.begin(); mi != mVCA_postmap.end(); ++mi) {
			int from_m = mi->first;

			map<int, map<int, set<int> > >::iterator statei; // over all source states
			for(statei = mi->second.begin(); statei != mi->second.end(); ++statei) {
				int from_state = statei->first;

				map<int, set<int> >::iterator labeli; // over all labels
				for(labeli = statei->second.begin(); labeli != statei->second.end(); ++labeli) {
					int mVCA_label = labeli->first;
					if(from_m + base_automaton->alphabet_get_direction(mVCA_label) < 0)
						continue; // skip invalid transitions, where m < 0

					list<int> next_pds_config;
					next_pds_config = get_next_stack_top(from_m, base_automaton->get_m_bound(),
							base_automaton->alphabet_get_direction(mVCA_label));

					set<int>::iterator desti; // over all destinations
					for(desti = labeli->second.begin(); desti != labeli->second.end(); ++desti) {
						int to_state = *desti;

						set<pair<int, list<int> > > destinations;
						set<pair<int, list<int> > >::iterator di;

						destinations = run_transition_accumulate(to_state, next_pds_config);

						for(di = destinations.begin(); di != destinations.end(); ++di) {
							if(!transition_exists(from_state, from_m, di->first)) {
								pa_transition_target tr;

								tr.dst = di->first;
								tr.mVCA_word = di->second;
								tr.mVCA_word.push_front(mVCA_label);

								transitions[from_state][from_m].insert( tr );
								new_transition_added = true;
					/*
								printf("PDS transition <%d, .%d.> -> <%d, %s> label %d: "
									"induces PA-transition %d -> %d label %d [PDS run %s]\n",
										from_state,	from_m,
										to_state,	word2string(next_pds_config).c_str(),
										mVCA_label,
										from_state,	di->first, from_m,
										word2string(tr.mVCA_word).c_str()
									);
					*/
							}
						}
					}
				}
			}
		}
	}

	return true;
}

list<int> p_automaton::get_shortest_valid_mVCA_run(int state, int m, bool & reachable) const
{
	list<int> ret;

	reachable = false;
	if(!valid || final.empty())
		return ret;

	list<int> cfg = get_config(state, m);
	int first_state = cfg.front();
	cfg.pop_front();

	set<pair<int, list<int> > > targets = run_transition_accumulate(first_state, cfg);
	set<pair<int, list<int> > >::iterator ti;

	for(ti = targets.begin(); ti != targets.end(); ++ti) {
		if(final.find(ti->first) != final.end()) {
			if(!reachable) {
				reachable = true;
				ret = ti->second;
			} else {
				if(ret.size() >= ti->second.size())
					ret = ti->second;
			}
		}
	}

	return ret;
}

string p_automaton::visualize() const
{
	string ret;
	char buf[128];
	set<int>::iterator si;

	if(!valid)
		return ret;

	ret += "digraph p_automaton {\n"
			"\tgraph[fontsize=8]\n"
			"\trankdir=TD;\n"
			"\tsize=8;\n"
			"\n"; // header

	// add initial states
	if(highest_initial_state >= 0) {
		ret += "\tnode [shape=circle, color=blue, style=\"filled\"];";
		for(int i = 0; i <= highest_initial_state; ++i) {
			snprintf(buf, 128, " p%d", i);
			ret += buf;

		}
		ret += ";\n";
	}
	// add normal states
	if(highest_initial_state < state_count) {
		ret += "\tnode [shape=circle, color=black, style=\"\"];";
		bool some = false;
		for(int i = highest_initial_state + 1; i < state_count; ++i) {
			if(final.find(i) == final.end()) {
				snprintf(buf, 128, " q%d", i);
				ret += buf;
				some = true;
			}
		}
		if(some)
			ret += ";\n";
		else
			ret += "\n";
	}
	// add final states
	if(!final.empty()) {
		ret += "\tnode [shape=doublecircle, color=black, style=\"\"];";
		for(si = final.begin(); si != final.end(); ++si) {
			snprintf(buf, 128, " q%d", *si);
			ret += buf;
		}
		ret += ";\n";
	}

	ret += "\n";

	// add transitions
	map<int, map<int, set<pa_transition_target> > >::const_iterator statei;
	map<int, set<pa_transition_target> >::const_iterator labeli;
	set<pa_transition_target>::const_iterator dsti;
	for(statei = transitions.begin(); statei != transitions.end(); ++statei) {
		for(labeli = statei->second.begin(); labeli != statei->second.end(); ++labeli) {
			for(dsti = labeli->second.begin(); dsti != labeli->second.end(); ++dsti) {
				if(dsti->mVCA_word.empty())
					snprintf(buf, 128, "\t%c%d -> %c%d [ label=\"%d\" ];\n",
							(statei->first <= highest_initial_state) ? 'p' : 'q', statei->first,
							(dsti->dst <= highest_initial_state) ? 'p' : 'q', dsti->dst,
							labeli->first);
				else
					snprintf(buf, 128, "\t%c%d -> %c%d [ label=\"%d [%s]\", color=\"#00aa00\" ];\n",
							(statei->first <= highest_initial_state) ? 'p' : 'q', statei->first,
							(dsti->dst <= highest_initial_state) ? 'p' : 'q', dsti->dst,
							labeli->first, word2string(dsti->mVCA_word).c_str());
				ret += buf;
			}
		}
	}


	ret += "};\n"; // footer

	return ret;
}






int p_automaton::new_state()
{
	int n = state_count;
	++state_count;
	return n;
}
list<int> p_automaton::get_config(int state, int m) const
// calculates a PDS configuration for given <state, m>.
// the result is a list<int> where ret.front() is the state and
// the rest of the list is the PDS stack (front is top).
{
	list<int> config;

	if(state < 0 || state >= base_automaton->get_state_count())
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
}
bool p_automaton::transition_exists(int from_state, int label, int to_state)
{
	set<pa_transition_target>::const_iterator tri;
	for(tri = transitions[from_state][label].begin(); tri != transitions[from_state][label].end(); ++tri)
		if(tri->dst == to_state)
			return true;
	return false;
}
set<int> p_automaton::run_transition(int from_state, int label)
{
	set<int> ret;
	set<pa_transition_target>::iterator tri;
	for(tri = transitions[from_state][label].begin(); tri != transitions[from_state][label].end(); ++tri)
		ret.insert(tri->dst);
	return ret;
}

set< pair<int, list<int> > > p_automaton::run_transition_accumulate(int from_state, int label, list<int> current_mVCA_run) const
{
	set< pair<int, list<int> > > ret;

	map<int, map<int, set<pa_transition_target> > >::const_iterator mmsi;
	map<int, set<pa_transition_target> >::const_iterator msi;
	set<pa_transition_target>::iterator tri;

	mmsi = transitions.find(from_state);
	if(mmsi != transitions.end()) {
		msi = mmsi->second.find(label);
		if(msi != mmsi->second.end()) {
			for(tri = msi->second.begin(); tri != msi->second.end(); ++tri)
				ret.insert(  pair<int, list<int> >(tri->dst, current_mVCA_run + tri->mVCA_word)  );
		}
	}

	return ret;
}
set< pair<int, list<int> > > p_automaton::run_transition_accumulate(int from_state, list<int> word) const
{
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
}


}; // end  of namespace libmVCA

