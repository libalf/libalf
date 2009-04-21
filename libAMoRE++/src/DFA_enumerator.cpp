/* $Id$
 * vim: fdm=marker
 *
 * amore++
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>

#include <amore++/DFA_enumerator.h>

namespace amore {

using namespace std;

DFA_enumerator::DFA_enumerator(int state_count, int alphabet_size)
{{{
	transition tr;
	int state;
	int sigma;

	completed = false;

	if(state_count < 2) {
		cout << "DFA_enumerator :: automaton_enumerator with state_count < 2 does not make any sense!\n";
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

bool DFA_enumerator::next(bool exact_state_count)
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

deterministic_finite_automaton * DFA_enumerator::derive()
{{{
	deterministic_finite_automaton * dfa;

	set<int> fini;
	transition_set trs;
	unsigned int i;

	for(i = 0; i < final.size(); i++)
		if(final[i])
			fini.insert(i);

	for(i = 0; i < transitions.size(); i++)
		trs.insert(transitions[i]);

	dfa = new deterministic_finite_automaton;
	if(!dfa->construct(alphabet_size, state_count, initial, fini, trs)) {
		delete dfa;
		return NULL;
	} else {
		return dfa;
	}
}}}

}; // end of namespace amore

