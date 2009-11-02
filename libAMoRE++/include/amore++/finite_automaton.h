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

#ifndef __amore_finite_automaton_h__
# define __amore_finite_automaton_h__

#include <list>
#include <set>
#include <map>
#include <string>

namespace amore {

using namespace std;


class automaton_run {
	public:
		list<int> prefix;
		int state;

		automaton_run()
		{ state = 0; };

		automaton_run(int first_state)
		{ state = first_state; };
};


class finite_automaton {
	public:
		virtual ~finite_automaton() = 0;

		// create a copy
		virtual finite_automaton * clone() = 0;
		// check if automaton is deterministic
		virtual bool is_deterministic() = 0;

		// LANGUAGE/AUTOMATON OPERATIONS

		// count (all) states
		virtual unsigned int get_state_count() = 0;
		// size of alphabet [0..n-1] \cap |N
		virtual unsigned int get_alphabet_size() = 0;

		// get and set initial and final states.
		// NOTE that in case of a DFA, setting multiple initial states will result in the LAST state being initial.
		// (according to set order)
		virtual set<int> get_initial_states() = 0;
		virtual set<int> get_final_states() = 0;
		virtual void set_initial_states(set<int> &states) = 0;
		virtual void set_final_states(set<int> &states) = 0;

		// check if set contains initial or final states
		virtual bool contains_initial_states(set<int> states) = 0;
		virtual bool contains_final_states(set<int> states) = 0;

		// get all successor or predecessor states to a state-set
		virtual set<int> successor_states(set<int> states) = 0;
		virtual set<int> predecessor_states(set<int> states) = 0;
		// get all successor or predecessor states to a state-set via a specifically labelled transition
		virtual set<int> successor_states(set<int> states, int label) = 0;
		virtual set<int> predecessor_states(set<int> states, int label) = 0;

		// calculate single-label transition or multiple-label run for a set of starting states
		virtual set<int> transition(set<int> from, int label)
		{   return successor_states(from, label);   };
		virtual set<int> run(set<int> from, list<int>::iterator word, list<int>::iterator word_limit);

		// get shortest run (i.e. word) from a state in <from> to a state in <to>
		// reachable = false if state is not reachable.
		virtual list<int> shortest_run(set<int> from, set<int> &to, bool &reachable) = 0;
		// check if states <to> are reachable from states <from>
		virtual bool is_reachable(set<int> &from, set<int> &to) = 0;

		// get a random sample word from this automaton (usually the shortest word in L)
		virtual list<int> get_sample_word(bool & is_empty) = 0;
		// test if word is contained in language of this automaton
		virtual bool contains(list<int> & word);


		// UNARY LANGUAGE TESTS

		// is the language of this automaton empty (resp. universal)?
		virtual bool is_empty() = 0;
		virtual bool is_universal() = 0;


		// UNARY LANGUAGE OPERATIONS

		// get complement language
		virtual void lang_complement() = 0;
		// get reversed language
		virtual finite_automaton * reverse_language() = 0;


		// UNARY AUTOMATON OPERATIONS

		// minimize automaton
		virtual void minimize() = 0;
		// get all states that may be merged into a negative sink
		virtual set<int> negative_sink();
		// get nondeterministic automaton
		virtual finite_automaton * nondeterminize() = 0;
		// an automaton is deterministic if it has only one initial state and, for every
		// letter, every state has at most one successor.
		virtual finite_automaton * determinize() = 0;
		// an automaton is co-deterministic if it has only one final state and, for every
		// letter, every state has at most one predecessor.
		virtual finite_automaton * co_determinize(bool minimize = true);


		// BINARY LANGUAGE TESTS

		// test if this == other
		virtual bool operator==(finite_automaton &other) = 0;
		// test if this is a subset of other
		virtual bool lang_subset_of(finite_automaton &other) = 0;
		// test if this and other are disjoint
		virtual bool lang_disjoint_to(finite_automaton &other) = 0;


		// BINARY LANGUAGE OPERATIONS

		// this+b
		virtual finite_automaton * lang_union(finite_automaton &other) = 0;
		// this AND b
		virtual finite_automaton * lang_intersect(finite_automaton &other) = 0;
		// this\b
		virtual finite_automaton * lang_difference(finite_automaton &other) = 0;
		// (this\other) + (other\this)
		virtual finite_automaton * lang_symmetric_difference(finite_automaton &other) = 0;
		// this.b
		virtual finite_automaton * lang_concat(finite_automaton &other) = 0;

		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized automaton>
		//	string length (not in bytes but in int32_t; excluding this length field)
		//	is deterministic? (1 if so, 0 otherwise)
		//	alphabet size
		//	state count
		//	number of initial states
		//	for each initial state:
		//		state id
		//	number of final states
		//	for each final state:
		//		state id
		//	number of transitions
		//	for each transition:
		//		source state id
		//		label (-1 for epsilon)
		//		destination state id
		// </serialized automaton>
		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;

		// construct a new automaton with states 0..state_count-1
		//
		// states are named 0 .. state_count-1,
		// transition attributes are 0 .. alphabet_size-1,
		// an epsilon transition is denoted as -1
		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int,int>, int> &transitions);

		virtual string generate_dotfile(bool exclude_negative_sinks);
		virtual string generate_dotfile()
		{ return generate_dotfile(false); };
};


// assemble an automaton from compounds:
finite_automaton * construct_amore_automaton(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int,int>, int> &transitions);


// automatically construct the specific automaton (NFA or DFA) during deserialization:
finite_automaton * deserialize_amore_automaton(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);


}; // end namespace amore

#endif

