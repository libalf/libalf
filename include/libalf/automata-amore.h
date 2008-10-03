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

#include <libalf/automata.h>
#include <amore/ext.h>
// at some point we need to call amore::initbuf() before using any amore stuff
// and amore::freebuf() at the end.

class amore_automata : public automata {
	private:
		dfa dfa_p;
		nfa nfa_p;

	public:
		automata() {
			dfa_p = NULL;
			nfa_p = NULL;
		}

		automata(enum automata_type type) {
			if(type == NONDETERMINISTIC_FINITE_AUTOMATA) {
				dfa_p = NULL;
				nfa_p = newnfa();
			}
			if(type == DETERMINISTIC_FINITE_AUTOMATA) {
				dfa_p = newdfa();
				nfa_p = NULL;
			};
		}

		virtual ~automata() {
			if(dfa_p)
				freedfa(dfa_p);
			if(nfa_p)
				freenfa(nfa_p);
		};

		virtual enum automata_type get_type() {
			if(nfa_p)
				return NONDETERMINISTIC_FINITE_AUTOMATA;
			if(dfa_p)
				return DETERMINISTIC_FINITE_AUTOMATA;

			return NO_AUTOMATA;
		}

}


}; // end namespace libalf
