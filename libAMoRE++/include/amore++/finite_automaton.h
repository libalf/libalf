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
 * (c) 2008,2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
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
		{{{
			state = 0;
		}}}

		automaton_run(int first_state)
		{{{
			state = first_state;
		}}}
};

class transition {
	public:
		class comparator { public: bool operator() (transition t1, transition t2) {return t1 < t2;} };
	public:
		int source;		// source node
		int label;		// attribute of transition (epsilon == -1)
		int destination;	// destination node

		transition() {
			source = -1;
			label = -1;
			destination = -1;
		}

		bool operator<(const transition & t)
		{{{
			if(this->source != t.source)   return (this->source < t.source);
			if(this->destination != t.destination)   return (this->destination < t.destination);
			return (this->label < t.label);
		}}}

		bool operator==(const transition &other)
		{{{
			return ((source == other.source) && (label == other.label) && (destination == other.destination));
		}}}
};

inline bool deterministic_transitions(const transition & t1, const transition & t2)
{ return (t1.source != t2.source) || (t1.label != t2.label) || (t1.destination == t2.destination); }

typedef set<transition, transition::comparator> transition_set;

class finite_automaton {
	public:
		virtual ~finite_automaton() = 0;

		virtual finite_automaton * clone() = 0;

		// LANGUAGE/AUTOMATON OPERATIONS
		virtual unsigned int get_state_count() = 0;
		virtual unsigned int get_alphabet_size() = 0;

		virtual set<int> get_initial_states() = 0;
		virtual set<int> get_final_states() = 0;

		// these can be used to create prefix- and postfix-languages for states
		// (e.g. residual languages)
		// note that setting new initial/final states will remove the attribute from former
		// states. in case of setting multiple initial states for a DFA, the last one
		// will be the only initial states (all others will NOT be initial).
		virtual void set_initial_states(set<int> &states) = 0;
		virtual void set_final_states(set<int> &states) = 0;

		// get shortest run from a state in <from> to a state in <to>
		// reachable = false if state is not reachable.
		virtual list<int> shortest_run(set<int> from, set<int> &to, bool &reachable) = 0;

		// are states <to> reachable from states <from> ?
		virtual bool is_reachable(set<int> &from, set<int> &to) = 0;

		// get a random sample word from this automaton
		virtual list<int> get_sample_word(bool & is_empty) = 0;

		// UNARY TESTS
		// is the language of this automaton empty (resp. universal)?
		virtual bool is_empty() = 0;
		virtual bool is_universal() = 0;

		// BINARY TESTS
		// test if this == other
		virtual bool operator==(finite_automaton &other) = 0;

		// test if this is a subset of other
		virtual bool lang_subset_of(finite_automaton &other) = 0;
		// test if this and other are disjoint
		virtual bool lang_disjoint_to(finite_automaton &other) = 0;

		// calculate single-label transition or multiple-label run for a set of starting states
		virtual set<int> transition(set<int> from, int label) = 0;
		virtual set<int> run(set<int> from, list<int>::iterator word, list<int>::iterator word_limit);

		// test if word is contained in language of this
		virtual bool contains(list<int> & word);

		// UNARY OPERATIONS
		virtual void minimize() = 0;
		virtual void lang_complement() = 0;

		// BINARY OPERATIONS
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

		virtual bool is_deterministic() = 0;

		virtual finite_automaton * nondeterminize() = 0;
		virtual finite_automaton * determinize() = 0;

		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized automaton>
		//	string length (not in bytes but in int32_t; excluding this length field)
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

		virtual string generate_dotfile();
};


// assemble an automaton from compounds:
finite_automaton * construct_amore_automaton(bool is_dfa, int alphabet_size, int state_count, set<int> &initial, set<int> &final, multimap<pair<int,int>, int> &transitions);


// automatically construct the specific automaton (NFA or DFA) during deserialization:
finite_automaton * deserialize_amore_automaton(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit);



}; // end namespace amore

#endif

