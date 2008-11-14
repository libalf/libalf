/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __libalf_automata_h__
# define __libalf_automata_h__

#include <list>
#include <set>
#include <string>

namespace libalf {

using namespace std;

enum automaton_type {
	FINITE_AUTOMATON,
	FINITE_LANGUAGE_AUTOMATON,
	NONDETERMINISTIC_FINITE_AUTOMATON,
	DETERMINISTIC_FINITE_AUTOMATON
};

enum automaton_implementation {
	IMP_NO_IMPLEMENTATION,
	IMP_AMORE
};

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

class automaton_run_less : binary_function<automaton_run, automaton_run, bool> {
	public:
		bool operator()(automaton_run first, automaton_run second)
		{
			return true;
		};
};

// an automaton implementation may throw this exception in case
// a function/operation is not defined
/*
class automaton_implementation_exception {
	virtual ~automaton_implementation_exception() { };
	virtual const char* what() const throw() = 0;
};
*/

class finite_automaton {
	public:
		virtual ~finite_automaton() { };

		virtual enum automaton_type get_type() {
			return FINITE_AUTOMATON;
		}

		virtual enum automaton_implementation get_implementation() {
			return IMP_NO_IMPLEMENTATION;
		}

		virtual finite_automaton * clone() = 0;
};

class transition {
	public:
		int source;		// source node
		int label;		// attribute of transition (epsilon == -1)
		int destination;	// destination node

		transition() {
			source = -1;
			label = -1;
			destination = -1;
		}

		bool operator==(transition &other)
		{{{
			return ((source == other.source) && (label == other.label) && (destination == other.destination));
		}}}

		bool operator<<(transition &other)
		{{{
			return ((source == other.source) && (label == other.label) && (destination != other.destination));
		}}}
};

class finite_language_automaton : public finite_automaton {
	public:
		virtual ~finite_language_automaton() { };

		virtual enum automaton_type get_type() {
			return FINITE_LANGUAGE_AUTOMATON;
		}

		virtual finite_language_automaton * clone() = 0;

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
		virtual bool operator==(finite_language_automaton &other) = 0;
		// test if this includes subautomaton
		virtual bool includes(finite_language_automaton &subautomaton) = 0;

		// test if word is contained in language of this
		virtual bool contains(list<int>&) = 0;

		// UNARY OPERATIONS
		virtual void minimize() = 0;
		virtual void lang_complement() = 0;

		// BINARY OPERATIONS
		// this+b
		virtual finite_language_automaton * lang_union(finite_language_automaton &other) = 0;
		// this AND b
		virtual finite_language_automaton * lang_intersect(finite_language_automaton &other) = 0;
		// (this+b) - (this AND b)
		virtual finite_language_automaton * lang_symmetric_difference(finite_language_automaton &other) = 0;
		// this-b
		virtual finite_language_automaton * lang_difference(finite_language_automaton &other) = 0;
		// this.b
		virtual finite_language_automaton * lang_concat(finite_language_automaton &other) = 0;

		virtual finite_language_automaton * nondeterminize() = 0;
		virtual finite_language_automaton * determinize() = 0;

		// format for serialization:
		// all values in NETWORK BYTE ORDER!
		// <serialized automaton>
		//	int alphabet size
		//	int state count
		//	int number of initial states
		//	for each initial state:
		//		int state id
		//	int number of final states
		//	for each final state:
		//		int state id
		//	int number of transitions
		//	for each transition:
		//		int source state id
		//		int label (-1 for epsilon)
		//		int destination state id
		// </serialized automaton>
		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t> &automaton) = 0;

		// the following two functions use serialize() and deserialize() and are thus implementation-independent:
		// please stick to construct for constructing automata, not to deserialize, as the format or serialize may change

		// construct a new automaton with states 0..state_count-1
		//
		// states are named 0 .. state_count-1,
		// transition attributes are 0 .. alphabet_size-1,
		// an epsilon transition is denoted as alphabet_size
		virtual bool construct(int alphabet_size, int state_count, list<int> &start, list<int> &final, list<transition> &transitions);

		virtual string generate_dotfile();
};

class nondeterministic_finite_automaton;

class deterministic_finite_automaton : public finite_language_automaton {
	public:
		virtual ~deterministic_finite_automaton() { };

		virtual enum automaton_type get_type() {
			return DETERMINISTIC_FINITE_AUTOMATON;
		}
};

class nondeterministic_finite_automaton : public finite_language_automaton {
	public:
		virtual ~nondeterministic_finite_automaton() { };

		virtual enum automaton_type get_type() {
			return NONDETERMINISTIC_FINITE_AUTOMATON;
		}

		virtual void epsilon_closure(set<int> & states) = 0;
};



}; // end namespace libalf

#endif

