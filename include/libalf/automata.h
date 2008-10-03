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

namespace libalf {

enum automata_type {
	NO_AUTOMATA,
	NONDETERMINISTIC_FINITE_AUTOMATA,
	DETERMINISTIC_FINITE_AUTOMATA
};

// an automata implementation may throw this exception in case
// a function/operation is not defined
class automata_implementation_exception {
	virtual const char* what() const throw() = 0;
}

// derive implementations from this interface (e.g. the amore implementation)
class automata {
	public:
		virtual ~automata();

		virtual enum automata_type get_type() = 0;

};

}; // end namespace libalf

