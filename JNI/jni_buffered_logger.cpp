/* $Id: jni_algorithm_angluin.cpp 501 2009-04-20 15:38:54Z neider $
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

#include <libalf/logger.h>

#include <jni.h>

#include "jni_buffered_logger.h"

using namespace std;
using namespace libalf;

JNIEXPORT jlong JNICALL Java_de_libalf_jni_BufferedLogger_init__ (JNIEnv *env, jobject obj) {
	/*
	 * Return the new object
	 */
	buffered_logger* logger = new buffered_logger();
	return ((jlong)logger);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_BufferedLogger_init__IZ (JNIEnv *env, jobject obj, jint minimal_loglevel, jboolean log_algorithm) {
	// Select log level
	enum logger_loglevel loglevel;

	switch(minimal_loglevel) {
	case 0:
		loglevel = LOGGER_INTERNAL;
		break;
	case 1:
		loglevel = LOGGER_ERROR;
		break;
	case 2:
		loglevel = LOGGER_WARN;
		break;
	case 3:
		loglevel = LOGGER_INFO;
		break;
	case 4:
		loglevel = LOGGER_DEBUG;
		break;
	case 5:
		loglevel = LOGGER_ALGORITHM;
		break;
	default:
		loglevel = LOGGER_ALGORITHM;
		break;
	}

	/*
	 * Return the new object
	 */
	buffered_logger* logger = new buffered_logger(loglevel, log_algorithm);
	return ((jlong)logger);
}

JNIEXPORT jstring JNICALL Java_de_libalf_jni_BufferedLogger_receive_1and_1flush (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the logger object
	buffered_logger *logger = (buffered_logger*)pointer;

	// Get string
	string* str;
	str = logger->receive_and_flush();

	//Convert string
	const char* c = str->c_str();

	return env->NewStringUTF(c);
}