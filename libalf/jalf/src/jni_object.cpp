/* $Id: jni_libalf_object.cpp 490 2009-04-16 20:40:34Z davidpiegdon $
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
 * (c) 2009 by Daniel Neider, Chair of Computer Science 2 and 7, RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 */

#include <string>

#include "jni_tools.h"

#include <libalf/alf.h>

#include <jni.h>

#include "jni_object.h"

using namespace std;
using namespace libalf;

JNIEXPORT jstring JNICALL JNICALL Java_de_libalf_jni_JNIObject_getLibALFVersion (JNIEnv *env, jclass cl) {
	// Get string
	string str;
	str = libalf_version();

    //Convert string
    const char* c = str.c_str();

    return env->NewStringUTF(c);
}
