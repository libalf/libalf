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

#include <map>
#include <set>
#include <string>
#include <list>

#include <jni.h>

using namespace std;

jintArray basic_string2jintArray(JNIEnv *, basic_string<int32_t>);

jintArray list_int2jintArray(JNIEnv *, list<int>);

jobject create_transition(JNIEnv*, int, int, int);

jobject convertAutomaton(JNIEnv* env, bool is_dfa, int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);

