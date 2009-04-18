/* $Id$
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */

#include <string>
#include <list>

#include <libalf/automaton_constructor.h>

#include <jni.h>

using namespace std;
using namespace libalf;

jintArray basic_string2jintArray(JNIEnv *, basic_string<int32_t>);

jintArray list_int2jintArray(JNIEnv *, list<int>);

jobject create_transition(JNIEnv*, int, int, int);

jobject convertAutomaton(JNIEnv*, basic_automaton_holder*);
