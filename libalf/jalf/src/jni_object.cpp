/* $Id: jni_libalf_object.cpp 490 2009-04-16 20:40:34Z davidpiegdon $
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
