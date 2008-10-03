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
	NONDETERMINISTIC_FINITE_AUTOMATA,
	DETERMINISTIC_FINITE_AUTOMATA
};

// derive implementations from this interface (e.g. the amore implementation)
class automata {
	public:
		virtual ~automata();

		virtual enum automata_type get_type() = 0;

};

}; // end namespace libalf

