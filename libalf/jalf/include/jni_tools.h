/* $Id: jni_tools.h 814 2009-09-04 08:28:54Z stefanschulz $
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
 * (c) 2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

#include <map>
#include <set>
#include <string>
#include <list>

#ifdef _WIN32
#include <winsock.h>
#include <stdio.h>
#else
#include <arpa/inet.h>
#endif

#include <jni.h>

using namespace std;

jintArray basic_string2jintArray_tohl(JNIEnv *, basic_string<int32_t>);

jintArray basic_string2jintArray(JNIEnv *, basic_string<int32_t>);

jintArray list_int2jintArray(JNIEnv *, list<int>);

jobject create_transition(JNIEnv*, int, int, int);

jobject convertAutomaton(JNIEnv* env, bool is_dfa, int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions);

