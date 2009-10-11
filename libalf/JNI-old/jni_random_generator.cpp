/* $Id: jni_random_generator.cpp 567 2009-05-03 17:48:25Z davidpiegdon $
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2009 by Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 * and 2009 by David R. Piegdon, i2 Informatik RWTH-Aachen
 *     <david-i2@piegdon.de>
 *
 */

#include <map>
#include <set>
#include <string>
#include <list>
#include <iostream>

#include "jni_tools.h"
#include "jni_random_generator.h"

#include <jni.h>

#include <liblangen/dfa_randomgenerator.h>

using namespace std;
using namespace liblangen;

static dfa_randomgenerator RAgen;

JNIEXPORT jobject JNICALL Java_de_libalf_jni_RandomGenerator_createRandomDFA_1private (JNIEnv *env, jclass cl, jint numberOfStates, jint alphabetSize) {
	/*
	 * Get the random DFA
	 */
	set<int> initial;
	set<int> final;
	multimap<pair<int, int>, int> transitions; 
	bool is_dfa;
	int resultNumberOfStates, resultAlphabetSize;
	RAgen.generate(alphabetSize, numberOfStates, is_dfa, resultAlphabetSize, resultNumberOfStates, initial, final, transitions);

	/*
	 * Convert to Java Basic Automaton
	 */
	return convertAutomaton(env, is_dfa, resultAlphabetSize, resultNumberOfStates, initial, final, transitions);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_RandomGenerator_discardTables (JNIEnv *env, jclass cl) {
	return RAgen.discard_tables();
}

