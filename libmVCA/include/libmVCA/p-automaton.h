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

#ifndef __libmvca_p_automaton_h__
# define __libmvca_p_automaton_h__

# include <vector>

# include <libmVCA/mVCA.h>

namespace libmVCA {

using namespace std;

/*
 * P-automata can be used to calculate Pre* of a regular set of configurations C of the underlying mVCA (or PDS).
 * The resulting P-automaton recognizes the exact set Pre*(C). With a refinement of the algorithm, it is possible
 * to find a shortest mVCA run for any of the configurations in Pre*(C).
 *
 * To use this, create a p-automaton and initialize it with the underlying mVCA:
 *
 *		p_automaton pa(my_mVCA);
 *
 * then add configurations that shall be accepted:
 * (for now, only a finite number of configurations is accepted, not a regular set)
 *
 *		pa.add_accepting_configuration( my_accepting_state, my_accepting_countervalue);
 *
 * tell the p-automaton to saturate itself, thus calculating Pre*(C).
 *
 *		pa.saturate_preSTAR();
 *
 * then, for any given mVCA configuration <q, m>, it can be checked if the set C is reachable:
 *
 *		bool reachable;
 *		list<int> run;
 *
 *		run = get_shortest_valid_mVCA_run(q, m, reachable);
 *
 *		if(reachable) {
 *			// (q,m) can reach a configuration in C via <run>.
 *		} else {
 *			// (q,m) can not reach any configuration in C.
 *		}
 */

// type to store transitions and keep track of some informations
class pa_transition_target {
	public:
		int dst;
		list<int> mVCA_word; // accumulated transitions in mVCA (!) that are required for this configuration change
	public:
		pa_transition_target()
		{ dst = -1; };
};

// so we can have sets of transition_targets:
bool operator<(const pa_transition_target first, const pa_transition_target second);
bool operator==(const pa_transition_target first, const pa_transition_target second);
bool operator>(const pa_transition_target first, const pa_transition_target second);


class p_automaton {
	private: // data
		bool valid;
		bool saturated;
		set< pair<int, int> > added_configurations;
		const mVCA * base_automaton;
		map<int, map<int, map<int, set<int> > > > mVCA_postmap; // m -> state -> label -> set<states>

		// the alphabet is different from the mVCA alphabet, as we operate over the
		// mVCA configuration, i.e. over <state, m>. the alphabet is m+1 in size.
		// 0 denotes the stack-empty symbol, 1 denotes m=1 and so on.
		// a special case is <alphabet_size-1>:
		// in case of m>=m_bound, the configuration looks like this:
		// <state, ++...+++#!>
		// where + is the highest label, # is the second highest label and ! is the lowest label (0).
		// the number <n> of ``+'' denotes that m is <n> more than #. thus removing a + denotes decrementing m.

		int alphabet_size;

		int state_count;
		// notice that the first <n> state are the initial states,
		// corresponding to states from the mVCA
		int highest_initial_state;
		set<int> final;
		// transitions :: state -> label -> destinations
		map<int, map<int, set<pa_transition_target> > > transitions;

	private: // methods
		int new_state();
		list<int> get_config(int state, int m) const;
		bool transition_exists(int from_state, int label, int to_state);
		set<int> run_transition(int from_state, int label);
		set< pair<int, list<int> > > run_transition_accumulate(int from_state, int label, list<int> current_mVCA_run) const;
		set< pair<int, list<int> > > run_transition_accumulate(int from_state, list<int> word) const;

	public: // methods
		p_automaton();
		p_automaton(const mVCA * base_automaton);
		void clear();

		bool initialize(const mVCA * base_automaton);
		bool add_accepting_configuration(int state, int m);

		bool saturate_preSTAR();

		list<int> get_shortest_valid_mVCA_run(int state, int m, bool & reachable) const;

		string visualize() const;
};


}; // end of namespace libmVCA

#endif // __libmvca_p_automaton_h__

