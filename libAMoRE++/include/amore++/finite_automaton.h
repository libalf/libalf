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

#include <stdint.h>

#include <list>
#include <set>
#include <map>
#include <string>

namespace amore {

class automaton_run {
	public:
		std::list<int> prefix;
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
		virtual finite_automaton * clone() const = 0;
		// check if automaton is deterministic
		virtual bool is_deterministic() const = 0;
		// get regex for language
		virtual std::string to_regex() const = 0;

		// LANGUAGE/AUTOMATON OPERATIONS

		// count (all) states
		virtual unsigned int get_state_count() const = 0;
		// size of alphabet [0..n-1] \cap |N
		virtual unsigned int get_alphabet_size() const = 0;

		// get and set initial and final states.
		// NOTE that in case of a DFA, setting multiple initial states will result in the LAST state being initial.
		// (according to set order)
		virtual std::set<int> get_initial_states() const = 0;
		virtual std::set<int> get_final_states() const = 0;
		virtual void set_initial_states(std::set<int> &states) = 0;
		virtual void set_final_states(std::set<int> &states) = 0;

		// check if set contains initial or final states
		virtual bool contains_initial_states(std::set<int> states) const = 0;
		virtual bool contains_final_states(std::set<int> states) const = 0;

		// get all successor or predecessor states to a state-set
		virtual std::set<int> successor_states(std::set<int> states) const = 0;
		virtual std::set<int> predecessor_states(std::set<int> states) const = 0;
		// get all successor or predecessor states to a state-set via a specifically labelled transition
		virtual std::set<int> successor_states(std::set<int> states, int label) const = 0;
		virtual std::set<int> predecessor_states(std::set<int> states, int label) const = 0;

		// epsilon-closure functions: (obviously only applicable to automata with epsilon-transitions)
		// apply epsilon-closure to state-set (i.e. include states that may be reached from those via epsilon)
		virtual void epsilon_closure(std::set<int> & states) const = 0;
		// apply inverted epsilon-closure to state-set (i.e. include states from whom these states may be reached via epsilon)
		virtual void inverted_epsilon_closure(std::set<int> & states) const = 0;

		// get mappings of all transitions. (post-transitions and pre-transitions, useful for fast, recurring calculations)
		// the mapping works as follows: map[current_state][label] = { predecessor- resp. successor-states }.
		// epsilon transitions are considered if the underlying automaton resolves epsilon-transitions
		// in predecessor_states() and successor_states().
		virtual void get_transition_maps(std::map<int, std::map<int, std::set<int> > > & premap, std::map<int, std::map<int, std::set<int> > > & postmap) const;

		// calculate single-label transition or multiple-label run for a set of starting states
		virtual std::set<int> transition(std::set<int> from, int label) const
		{   return successor_states(from, label);   };
		virtual std::set<int> run(std::set<int> from, std::list<int>::const_iterator word, std::list<int>::const_iterator word_limit) const;

		// get shortest run (i.e. word) from a state in <from> to a state in <to>
		// reachable = false if state is not reachable.
		virtual std::list<int> shortest_run(std::set<int> from, std::set<int> &to, bool &reachable) const = 0;
		// check if states <to> are reachable from states <from>
		virtual bool is_reachable(std::set<int> &from, std::set<int> &to) const = 0;

		// get a random sample word from this automaton (usually the shortest word in L)
		virtual std::list<int> get_sample_word(bool & is_empty) const = 0;
		// test if word is contained in language of this automaton
		virtual bool contains(std::list<int> & word) const;


		// UNARY LANGUAGE TESTS

		// is the language of this automaton empty (resp. universal)?
		virtual bool is_empty() const = 0;
		virtual bool is_universal() const = 0;


		// UNARY LANGUAGE OPERATIONS

		// get complement language
		virtual void lang_complement() = 0;
		// get reversed language
		virtual finite_automaton * reverse_language() const = 0;


		// UNARY AUTOMATON OPERATIONS

		// minimize automaton
		virtual void minimize() = 0;
		// get all states that may be merged into a negative sink
		virtual std::set<int> negative_sink() const;
		// get nondeterministic automaton
		virtual finite_automaton * nondeterminize() const = 0;
		// an automaton is deterministic if it has only one initial state and, for every
		// letter, every state has at most one successor.
		virtual finite_automaton * determinize() const = 0;
		// an automaton is co-deterministic if it has only one final state and, for every
		// letter, every state has at most one predecessor.
		virtual finite_automaton * co_determinize(bool minimize = true) const;

		// BINARY LANGUAGE TESTS

		// test if this == other
		virtual bool operator==(const finite_automaton &other) const = 0;
		// test if this is a subset of other
		virtual bool lang_subset_of(const finite_automaton &other) const = 0;
		// test if this and other are disjoint
		virtual bool lang_disjoint_to(const finite_automaton &other) const = 0;

		// BINARY LANGUAGE OPERATIONS

		// this+b
		virtual finite_automaton * lang_union(const finite_automaton &other) const = 0;
		// this AND b
		virtual finite_automaton * lang_intersect(const finite_automaton &other) const = 0;
		// this\b
		virtual finite_automaton * lang_difference(const finite_automaton &other) const = 0;
		// (this\other) + (other\this)
		virtual finite_automaton * lang_symmetric_difference(const finite_automaton &other) const = 0;
		// this.b
		virtual finite_automaton * lang_concat(const finite_automaton &other) const = 0;

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
		virtual std::basic_string<int32_t> serialize() const = 0;
		virtual bool deserialize(std::basic_string<int32_t>::const_iterator &it, std::basic_string<int32_t>::const_iterator limit) = 0;

		// construct a new automaton with states 0..state_count-1
		//
		// states are named 0 .. state_count-1,
		// transition attributes are 0 .. alphabet_size-1,
		// an epsilon transition is denoted as -1
		virtual bool construct(bool is_dfa, int alphabet_size, int state_count, std::set<int> &initial, std::set<int> &final, std::map<int, std::map<int, std::set<int> > > &transitions);

		// create dotfile from automaton:
		virtual std::string visualize(bool exclude_negative_sinks) const;
		virtual std::string visualize() const
		{ return visualize(false); };

	public:
		// following are antichain-based algorithms for universality, inclusion and equality-tests for languages.
		// depending on their performance, once implemented, is_universal(), lang_subset_of() and operator==()
		// should relay to them.
		// for nondeterministic automata, these are far more efficient than determinization + test.

		// Antichain-based algorithms. See
		//    M. De Wulf, L. Doyen, J.-F. Raskin
		//    Antichains: A New Algorithm for Checking Universality of Finite Automata

//		virtual bool antichain__is_universal(std::list<int> & counterexample) const;
		virtual bool antichain__is_equal(const finite_automaton &other, std::list<int> & counterexample) const;
		virtual bool antichain__is_superset_of(const finite_automaton &other, std::list<int> & counterexample) const;
	private:
		bool antichain__superset_check_winning_condition(std::set<int> & this_initial, std::set<int> & other_initial, const std::pair<int, std::pair< std::set<int>, std::list<int> > > & gamestate, std::list<int> & counterexample) const;
};


// assemble an automaton from compounds:
finite_automaton * construct_amore_automaton(bool is_dfa, int alphabet_size, int state_count, std::set<int> &initial, std::set<int> &final, std::map<int, std::map<int, std::set<int> > > &transitions);


// automatically construct the specific automaton (NFA or DFA) during deserialization:
finite_automaton * deserialize_amore_automaton(std::basic_string<int32_t>::const_iterator &it, std::basic_string<int32_t>::const_iterator limit);


}; // end namespace amore

#endif

