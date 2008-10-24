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

// FIXME: see below
#include <setjmp.h>

#include "libalf/alf.h"

using namespace std;
using namespace libalf;


// FIXME: remove setjmp/longjmp from libAMoRE, so this shit is not needed:
char itoc[28] = "@abcdefghijklmnopqrstuvwxyz";
jmp_buf _jmp;
char dummy[8];

int main()
{
	// create random automata

	// create oracle instance and teacher instance

	// create simple_observationtable and teach it the automata

	// once an automata is generated, test for equality with oracle_automata
	// if this test is ok, all worked well
	return 0;
}

