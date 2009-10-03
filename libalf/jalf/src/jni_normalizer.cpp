/* $Id: jni_normalizer.cpp 952
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

#include <libalf/normalizer.h>
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
	normalizer *norm = (normalizer*)pointer;

	// Convert
	jintArray arr = basic_string2jintArray(env, norm->serialize());

	return arr;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNINormalizer_deserialize (JNIEnv *env, jobject obj, jintArray serialization, jlong pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(serialization);
	jint *entry = env->GetIntArrayElements(serialization, 0);
	// Copy array
	int len = (int)length;
	basic_string<int32_t> ser;
	for(int i=0; i<len; i++) ser.push_back(((jint)entry[i]));
	// Clean
	env->ReleaseIntArrayElements(serialization, entry, 0);

	// Get the knowledgebase object
	normalizer *norm = (normalizer*)pointer;

	// Forward method call
	basic_string<int32_t>::iterator si;
	si = ser.begin();
	return norm->deserialize(si, ser.end());
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNINormalizer_destroy (JNIEnv *env, jobject obj, jlong pointer) {
	// Kill the normalizer
	delete (normalizer*)pointer;
}
