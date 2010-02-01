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

// type to store transitions and keep track of some informations
typedef struct {
	int dst;
	bool is_new;
	vector<int> new_used_transitions;
} pa_transition_target;

// so we can have sets of transition_targets:
bool operator<(const pa_transition_target first, const pa_transition_target second);
bool operator==(const pa_transition_target first, const pa_transition_target second);
bool operator>(const pa_transition_target first, const pa_transition_target second);




class p_automaton {
	// effectively this is an NFA
	private: // types

	private: // data
		bool valid;
		bool saturated;
		mVCA * base_automaton;

		int alphabet_size;

		int state_count;
		// notice that the first <n> state are the initial states,
		// corresponding to states from the mVCA
		set<int> initial;
		set<int> final;
		// transitions :: state -> set<target>
		map<int, set<pa_transition_target> > transitions;

	public: // methods
		p_automaton();
		p_automaton(mVCA * base_automaton);
		void clear();

		bool initialize(mVCA * base_automaton);
		bool add_accepting_configuration(int state, int m);

		bool saturate_preSTAR();

		list<int> check_acceptance(int state, int m, bool & reachable);
};

}; // end of namespace libmVCA

#endif // __libmvca_p_automaton_h__

