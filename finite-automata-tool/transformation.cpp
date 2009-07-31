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
			return true;
		case trans_mdfa:
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			/* fall through */
		case trans_minimize:
			automaton->minimize();
			return true;
		case trans_determinize:
			tmp = automaton->determinize();
			delete automaton;
			automaton = tmp;
			return true;
		case trans_rfsa:
			// FIXME: apply NL*
			
			break;
		case trans_universal:
			// FIXME: apply UL*
			
			break;
	}

	return false;
}

