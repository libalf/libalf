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

#ifndef __amore_automaton_h__
# define __amore_automaton_h__

#include <list>
#include <set>
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

typedef std::set<transition, transition::comparator> transition_set;

class finite_automaton {
	public:
		virtual ~finite_automaton() { };

		virtual finite_automaton * clone() = 0;

		// LANGUAGE/AUTOMATON OPERATIONS
		virtual int get_state_count() = 0;
		virtual int get_alphabet_size() = 0;

		// get a random sample word from this automaton
		virtual list<int> get_sample_word(bool & is_empty) = 0;

		// UNARY TESTS
		// is the language of this automaton empty?
		virtual bool is_empty() = 0;

		// BINARY TESTS
		// test if this == other
		virtual bool operator==(finite_automaton &other) = 0;

		// test if this is a subset of other
		virtual bool lang_subset_of(finite_automaton &other) = 0;
		// test if this and other are disjoint
		virtual bool lang_disjoint_to(finite_automaton &other) = 0;

		// test if word is contained in language of this
		virtual bool contains(list<int>&) = 0;

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

		// the following two functions use serialize() and deserialize() and are thus implementation-independent:
		// please stick to construct for constructing automata, not to deserialize, as the format or serialize may change
		// also note that start, final and transitions may contain the same values multiple times!

		// construct a new automaton with states 0..state_count-1
		//
		// states are named 0 .. state_count-1,
		// transition attributes are 0 .. alphabet_size-1,
		// an epsilon transition is denoted as alphabet_size
		virtual bool construct(int alphabet_size, int state_count, std::set<int> &start, std::set<int> &final, transition_set &transitions);

		virtual string generate_dotfile();
};

}; // end namespace amore

#endif

