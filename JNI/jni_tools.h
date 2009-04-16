/* $Id$
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *    and Daniel Neider, i7 Informatik RWTH-Aachen
 *        <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */

#include <string>
#include <list>

#include <jni.h>

using namespace std;

int32_t *basic_string2intarray(basic_string<int32_t> str);

jintArray basic_string2jintArray(JNIEnv *, basic_string<int32_t>);

jintArray list_int2jintArray(JNIEnv *, list<int>);