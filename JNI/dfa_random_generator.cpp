/* $Id$
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 * and David R. Piegdon, i2 Informatik RWTH-Aachen
 *     <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <string>
#include <list>
#include <iostream>

#include <jni.h>

#include <libalf/automaton_constructor.h>
#include <LanguageGenerator/DFArandomgenerator.h>

#include "alf_langen_glue.h"

using namespace std;
using namespace libalf;
using namespace LanguageGenerator;

static DFArandomgenerator RAgen;

bool create_random_DFA(int alphabet_size, int state_count, libalf::automaton_constructor & automaton)
{
	alf_langen_glue::langen_automaton_constructor_wrapper wrapper;

	wrapper.set_constructor(&automaton);

	return RAgen.generate(alphabet_size, state_count, wrapper);
}

void discard_tables()
{
	return RAgen.discard_tables();
}

