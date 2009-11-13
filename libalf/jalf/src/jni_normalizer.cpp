/* $Id: jni_normalizer.cpp 952
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

#include <string>

#include "jni_tools.h"

#include <libalf/normalizer_msc.h>

#include <jni.h>

#include "jni_normalizer.h"

using namespace std;
using namespace libalf;

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNINormalizer_init (JNIEnv *env, jobject obj) {
	/*
	 * Return the new object
	 */
	normalizer_msc *norm = new normalizer_msc;
	return ((jlong)norm);
}

JNIEXPORT jintArray JNICALL Java_de_libalf_jni_JNINormalizer_serialize (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the normalizer object
	normalizer_msc *norm = (normalizer_msc*)pointer;

	// Convert
	jintArray arr = basic_string2jintArray_tohl(env, norm->serialize());

	return arr;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNINormalizer_deserialize (JNIEnv *env, jobject obj, jintArray serialization, jlong pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(serialization);
	jint *entry = env->GetIntArrayElements(serialization, 0);
	// Copy array
	int len = (int)length;
	basic_string<int32_t> ser;
	for(int i=0; i<len; i++) ser.push_back(htonl(((jint)entry[i])));
	// Clean
	env->ReleaseIntArrayElements(serialization, entry, 0);

	// Get the knowledgebase object
	normalizer_msc *norm = (normalizer_msc*)pointer;

	// Forward method call
	basic_string<int32_t>::iterator si;
	si = ser.begin();
	if(!norm->deserialize(si, ser.end()))
		return false;

	return (si == ser.end());
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNINormalizer_destroy (JNIEnv *env, jobject obj, jlong pointer) {
	// Kill the normalizer
	delete (normalizer_msc*)pointer;
}

