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

#include <map>
#include <set>
#include <string>
#include <list>
#include <iostream>

#include <jni.h>

#include <LanguageGenerator/DFArandomgenerator.h>

using namespace std;
using namespace LanguageGenerator;

static DFArandomgenerator RAgen;

bool create_random_DFA(int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions)
{
	bool tmp;
	return RAgen.generate(alphabet_size, state_count, tmp, alphabet_size, state_count, initial, final, transitions);
}

void discard_tables()
{
	return RAgen.discard_tables();
}

