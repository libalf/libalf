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

#include <amore/vars.h>

using namespace std;
using namespace libalf;

int main()
{
	automata *atm = new automata_amore();
	logger *log = new ostream_logger(&cout, LOGGER_DEBUG, true);
	teacher<bool> *teach = new teacher_automata<bool>(*atm);

	simple_observationtable<bool> *ob = new simple_observationtable<bool>(*teach, *log, 2);

	// create random automata

	// create oracle instance and teacher instance

	// create simple_observationtable and teach it the automata

	// once an automata is generated, test for equality with oracle_automata
	// if this test is ok, all worked well

	delete ob;
	delete teach;
	delete log;
	delete atm;

	return 0;
}

