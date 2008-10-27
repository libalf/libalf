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
	NO_AUTOMATA,
	NONDETERMINISTIC_FINITE_AUTOMATA,
	DETERMINISTIC_FINITE_AUTOMATA
};

// an automata implementation may throw this exception in case
// a function/operation is not defined
/*
class automata_implementation_exception {
	virtual ~automata_implementation_exception() { };
	virtual const char* what() const throw() = 0;
};
*/

// derive implementations from this interface (e.g. the amore implementation)
class automata {
	public:
		virtual ~automata() { };

		virtual enum automata_type get_type() = 0;


		// GENERAL STUFF
		virtual automata * clone() = 0;

		virtual string * generate_dotfile() = 0;


		// LANGUAGE/AUTOMATA OPERATIONS

		// UNARY TESTS
		// is this language empty?
		virtual bool is_empty() = 0;
		// get a random sample word from this automata
		virtual list<int> get_sample_word() = 0;

		// BINARY TESTS
		// test if this == other
		virtual bool operator==(automata &other) = 0;
		// test if this includes subautomata
		virtual bool includes(automata &subautomata) = 0;
		// test if this is a subset of superautomata
		virtual bool is_subset_of(automata &superautomata) = 0;

		// test if word is contained in language of this
		virtual bool contains(list<int>) = 0;

		// UNARY OPERATIONS
		virtual void make_deterministic() = 0;
		virtual void make_undeterministic() = 0;
		virtual void minimize() = 0;
		virtual void lang_complement() = 0;

		// BINARY OPERATIONS
		// this+b
		virtual automata* lang_union(automata &other) = 0;
		// this AND b
		virtual automata* lang_intersect(automata &other) = 0;
		// (this+b) - (this AND b)
		virtual automata* lang_difference(automata &other) = 0;
		// this-b
		virtual automata* lang_without(automata &other) = 0;
		// this.b
		virtual automata* lang_concat(automata &other) = 0;

};

}; // end namespace libalf

#endif

