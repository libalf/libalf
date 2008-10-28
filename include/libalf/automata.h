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
#include <string>

namespace libalf {

using namespace std;

enum automata_type {
	FINITE_AUTOMATA,
	FINITE_LANGUAGE_AUTOMATON,
	NONDETERMINISTIC_FINITE_AUTOMATA,
	DETERMINISTIC_FINITE_AUTOMATA
};

enum automaton_implementation {
	IMP_NO_IMPLEMENTATION
	IMP_AMORE_AUTOMATA
};

// an automata implementation may throw this exception in case
// a function/operation is not defined
/*
class automata_implementation_exception {
	virtual ~automata_implementation_exception() { };
	virtual const char* what() const throw() = 0;
};
*/

class finite_automaton {
	public:
		virtual ~finite_automaton() { };

		virtual enum automata_type get_type() {
			return FINITE_AUTOMATA;
		}

		virtual enum automaton_implementation get_implementation() {
			return IMP_NO_IMPLEMENTATION;
		}

		virtual finite_automaton * clone() = 0;

		virtual string generate_dotfile() = 0;
}'

class finite_language_automaton : public finite_automaton {
	public:
		virtual ~finite_language_automaton() { };

		virtual enum automata_type get_type() {
			return FINITE_LANGUAGE_AUTOMATON;
		}

		virtual enum automaton_implementation get_implementation() {
			return IMP_NO_IMPLEMENTATION;
		}

		// LANGUAGE/AUTOMATON OPERATIONS

		// UNARY TESTS
		// is the language of this automaton empty?
		virtual bool is_empty() = 0;
		// get a random sample word from this automaton
		virtual list<int> get_sample_word() = 0;

		// BINARY TESTS
		// test if this == other
		virtual bool operator==(finite_automaton &other) = 0;
		// test if this includes subautomata
		virtual bool includes(finite_automaton &subautomata) = 0;
		// test if this is a subset of superautomata
		virtual bool is_subset_of(finite_automaton &superautomata) = 0;

		// test if word is contained in language of this
		virtual bool contains(list<int>) = 0;

		// UNARY OPERATIONS
		virtual void minimize() = 0;
		virtual void lang_complement() = 0;

		// BINARY OPERATIONS
		// this+b
		virtual finite_language_automaton * lang_union(automata &other) = 0;
		// this AND b
		virtual finite_language_automaton * lang_intersect(automata &other) = 0;
		// (this+b) - (this AND b)
		virtual finite_language_automaton * lang_difference(automata &other) = 0;
		// this-b
		virtual finite_language_automaton * lang_without(automata &other) = 0;
		// this.b
		virtual finite_language_automaton * lang_concat(automata &other) = 0;
};

class deterministic_finite_automaton : public finite_automaton {
	public:
		virtual enum automata_type get_type() {
			return DETERMINISTIC_FINITE_AUTOMATA;
		}

		virtual ~deterministic_finite_automaton() { };

		virtual finite_language_automaton * undeterminize() = 0;
}

class nondeterministic_finite_automaton : public finite_automaton {
	public:
		virtual ~nondeterministic_finite_automaton() { };

		virtual enum automata_type get_type() {
			return NONDETERMINISTIC_FINITE_AUTOMATA;
		}

		virtual finite_language_automaton * determinize() = 0;
}



}; // end namespace libalf

#endif

