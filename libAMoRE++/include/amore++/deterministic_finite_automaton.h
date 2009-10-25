/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libAMoRE++
 *
 * libAMoRE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libAMoRE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libAMoRE++.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __amore_deterministic_automaton_h__
# define __amore_deterministic_automaton_h__

#include <list>
#include <string>
#include <set>

#include <amore++/finite_automaton.h>

#ifdef LIBAMORE_LIBRARY_COMPILATION
# include <amore/nfa.h>
# include <amore/dfa.h>
#else
# define nfa void*
# define dfa void*
#endif

namespace amore {

using namespace std;

class deterministic_finite_automaton : public finite_automaton {
	private:
		dfa dfa_p;

	public:
		deterministic_finite_automaton();
		deterministic_finite_automaton(dfa a);



		virtual ~deterministic_finite_automaton();

		virtual deterministic_finite_automaton * clone();

		virtual unsigned int get_state_count();
		virtual unsigned int get_alphabet_size();
		virtual set<int> get_initial_states();
		virtual set<int> get_final_states();
		virtual void set_initial_states(set<int> &states);
		virtual void set_final_states(set<int> &states);
		virtual bool contains_initial_states(set<int> states);
		virtual bool contains_final_states(set<int> states);
		virtual set<int> successor_states(set<int> states);
		virtual set<int> successor_states(set<int> states, int label);
		virtual set<int> predecessor_states(set<int> states);
		virtual set<int> predecessor_states(set<int> states, int label);
		virtual list<int> shortest_run(set<int> from, set<int> &to, bool &reachable);
		virtual bool is_reachable(set<int> &from, set<int> &to);
		virtual list<int> get_sample_word(bool & is_empty);
		virtual bool is_empty();
		virtual bool is_universal();
		virtual bool operator==(finite_automaton &other);
		virtual bool lang_subset_of(finite_automaton &other);
		virtual bool lang_disjoint_to(finite_automaton &other);
		virtual set<int> transition(set<int> from, int label);
		virtual void minimize();
		virtual void lang_complement();
		virtual finite_automaton * reverse_language();
		virtual finite_automaton * lang_union(finite_automaton &other);
		virtual finite_automaton * lang_intersect(finite_automaton &other);
		virtual deterministic_finite_automaton * lang_difference(finite_automaton &other);
		virtual finite_automaton * lang_symmetric_difference(finite_automaton &other);
		virtual finite_automaton * lang_concat(finite_automaton &other);

		virtual bool is_deterministic();
		virtual finite_automaton * nondeterminize();
		virtual deterministic_finite_automaton * determinize();

		virtual basic_string<int32_t> serialize();
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);

		// overloading
		virtual bool contains(list<int> &word);
		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int,int>, int> &transitions);

		// new
		virtual void set_dfa(dfa a);
		virtual dfa get_dfa();
};

}; // end namespace amore

#ifndef LIBAMORE_LIBRARY_COMPILATION
# undef nfa
# undef dfa
#endif

#endif

