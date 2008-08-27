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

};

union automata_classes {
	NONDETERMINISTIC_FINITE_AUTOMATA,
	DETERMINISTIC_AUTOMATA
};

class automata {
	public:
		enum automata_type	type;
		union {
			NFA *nfa;
			DFA *dfa;
			// ...
		} a;
};

}; // end namespace libalf

