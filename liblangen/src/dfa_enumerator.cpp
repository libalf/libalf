/* $Id$
 * vim: fdm=marker
 *
 * This file is part of liblangen (LANguageGENerator)
 *
 * liblangen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblangen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with liblangen.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

/*
 * dfa_enumerator: enumerate over all accessible n-state automata with specific alphabet size
 */

#include <iostream>

#include <liblangen/dfa_enumerator.h>

namespace liblangen {

using namespace std;

dfa_enumerator::dfa_enumerator(int state_count, int alphabet_size)
{{{
	transition tr;
	int state;
	int sigma;

	completed = false;

	if(state_count < 2) {
		cout << "dfa_enumerator :: automaton_enumerator with state_count < 2 does not make any sense!\n";
		state_count = 2;
	}

	if(alphabet_size < 1) {
		cout << "automaton_enumerator with alphabet_size < 1 does not make any sense!\n";
		alphabet_size = 1;
	}

	this->state_count = state_count;
	this->alphabet_size = alphabet_size;

	// initial state will always be state 0.
	// this is ok due to renaming isomorphism.
	initial.insert(0);

	for(state = 0; state < state_count; state++) {
		tr.source = state;
		tr.destination = 0;
		for(sigma = 0; sigma < alphabet_size; sigma++) {
			tr.label = sigma;
			transitions.push_back(tr);
		}

		final.push_back(false);
	}
}}}

bool dfa_enumerator::next(bool exact_state_count)
{{{
	int i;
	bool aborted = false;

	for(i = final.size()-1; i >= 0; i--) {
		if(!final[i]) {
			final[i] = true;
			goto check_reachable;
		}

		final[i] = false;
	}
	// take care that at least one state is always final
	final[final.size() - 1] = true;

	for(i = transitions.size() - 1; i >= 0; i--) {
		if(transitions[i].destination < state_count - 1) {
			transitions[i].destination++;
			aborted = true;
			break;
		}

		transitions[i].destination = 0;
	}

	if(!aborted) {
		completed = true;
		return false;
	}

check_reachable:
	if(exact_state_count) {
		// check if all states are reachable.
		// otherwise skip this automaton.
		set<int> reachable;
		set<int>::iterator si;
		for(si = initial.begin(); si != initial.end(); si++)
			reachable.insert(*si);

		bool changed = true;

		while(changed) {
			changed = false;
			for(int state = 0; state < state_count; state++) {
				// skip all reachable
				if(reachable.find(state) != reachable.end())
					continue;

				// check if non-reachable state has transition from a reachable state
				for(si = reachable.begin(); si != reachable.end(); si++) {
					vector<transition>::iterator tri;

					for(tri = transitions.begin(); tri != transitions.end(); tri++) {
						if(tri->source == *si && tri->destination == state) {
							reachable.insert(state);
							changed = true;
						}
					}
				}
			}
		}

		for(int state = 0; state < state_count; state++)
			if(reachable.find(state) == reachable.end())
				return false;
	}
	return true;
}}}

bool dfa_enumerator::derive(bool &t_is_dfa, int &t_alphabet_size, int &t_state_count, std::set<int> &t_initial, std::set<int> &t_final, multimap<pair<int,int>, int> &t_transitions)
{{{
	unsigned int i;

	t_initial.clear();
	t_final.clear();
	t_transitions.clear();

	t_initial = initial;

	for(i = 0; i < final.size(); i++)
		if(final[i])
			t_final.insert(i);

	for(i = 0; i < transitions.size(); i++) {
		pair<int, int> trid;

		trid.first = transitions[i].source;
		trid.second = transitions[i].label;
		t_transitions.insert( pair<pair<int, int>, int>( trid, transitions[i].destination) );
	}

	t_is_dfa = true;
	t_alphabet_size = alphabet_size;
	t_state_count = state_count;
	return true;
}}}

}; // end of namespace liblangen

