/* $Id: jni_algorithm_angluin.cpp 492 2009-04-18 17:10:59Z neider $
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>
#include <libalf/algorithm_biermann_minisat.h>

#include <jni.h>

#include "jni_algorithm_biermann_minisat.h"

using namespace std;
using namespace libalf;

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmBiermannMiniSAT_init__JI (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new MiniSat_biermann<bool>(base, NULL, alphabet_size);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmBiermannMiniSAT_init__JIJ (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size, jlong logger_pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) logger_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new MiniSat_biermann<bool>(base, logger, alphabet_size);
	return ((jlong)algorithm);
}
