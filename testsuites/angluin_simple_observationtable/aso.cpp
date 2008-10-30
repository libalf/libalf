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

#include <iostream>

#include "libalf/alf.h"

#include "libalf/automata_amore.h"

#include <amore/vars.h>

using namespace std;
using namespace libalf;

int main()
{
	deterministic_finite_amore_automaton *atm;
	logger *log;
	teacher<bool> *teach;
	simple_observationtable<bool> *ob;

	atm = new deterministic_finite_amore_automaton();
	log = new ostream_logger(&cout, LOGGER_DEBUG, true);
	teach = new teacher_automaton<bool>(atm);
	ob = new simple_observationtable<bool>(teach, log, 2);

	// create random automaton

	// create oracle instance and teacher instance

	// create simple_observationtable and teach it the automaton

	// once an automaton is generated, test for equality with oracle_automaton
	// if this test is ok, all worked well

	delete ob;
	delete teach;
	delete log;
	delete atm;

	return 0;
}

