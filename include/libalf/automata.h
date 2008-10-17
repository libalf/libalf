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

namespace libalf {

enum automata_type {
	NO_AUTOMATA,
	NONDETERMINISTIC_FINITE_AUTOMATA,
	DETERMINISTIC_FINITE_AUTOMATA
};

// an automata implementation may throw this exception in case
// a function/operation is not defined
class automata_implementation_exception {
	virtual ~automata_implementation_exception() { };
	virtual const char* what() const throw() = 0;
};

// derive implementations from this interface (e.g. the amore implementation)
class automata {
	public:
		virtual ~automata() { };

		virtual enum automata_type get_type() = 0;

		// general stuff
		virtual automata* clone() = 0;

		// language/automata operations:

		// unary tests
		virtual bool is_empty() = 0;

		// binary tests
		virtual operator==(automata &other) = 0;
		virtual bool includes(automata &included) = 0;
		virtual bool is_subset_of(automata &included) = 0;

		// unary operations
		virtual void make_deterministic() = 0;
		virtual void lang_complement() = 0;

		// binary operations
		// a+b
		virtual automata* lang_union(automata &other) = 0;
		// a AND b
		virtual automata* lang_intersect(automata &other) = 0;
		// a+b - a AND b
		virtual automata* lang_difference(automata &other) = 0;
		// a-b
		virtual automata* lang_without(automata &other) = 0;
		// a.b
		virtual automata* lang_concat(automata &other) = 0;
};

}; // end namespace libalf

#endif

