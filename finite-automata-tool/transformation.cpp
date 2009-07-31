/* $Id$
 * vim: fdm=marker
 *
 * Finite Automata Tools
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>

#include "fat.h"

bool do_transformation(finite_automaton *& automaton, transformation trans)
{
	finite_automaton * tmp;
	switch(trans) {
		case trans_none:
cerr<<"T:none\n";
			return true;
		case trans_mdfa:
cerr<<"T:mdfa\n";
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			/* fall through */
		case trans_minimize:
cerr<<"T:min\n";
			automaton->minimize();
			return true;
		case trans_determinize:
cerr<<"T:det\n";
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			return true;
		case trans_rfsa:
cerr<<"T:rfsa\n";
			// FIXME: apply NL*

			break;
		case trans_universal:
cerr<<"T:uni\n";
			// FIXME: apply UL*
			break;
	}

	return false;
}

