/* $Id: jni_algorithm_angluin.cpp 501 2009-04-20 15:38:54Z neider $
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

#include <iostream>

#include <libalf/logger.h>

#include <jni.h>

#include "jni_buffered_logger.h"

using namespace std;
using namespace libalf;

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIBufferedLogger_init__ (JNIEnv *env, jobject obj) {
	/*
	 * Return the new object
	 */
	buffered_logger* logger = new buffered_logger();
	return ((jlong)logger);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIBufferedLogger_init__IZ (JNIEnv *env, jobject obj, jint minimal_loglevel, jboolean log_algorithm) {
	// Select log level
	enum logger_loglevel loglevel;

	switch(minimal_loglevel) {
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
	default:
		loglevel = LOGGER_DEBUG;
		break;
	}

	/*
	 * Return the new object
	 */
	buffered_logger* logger = new buffered_logger(loglevel, log_algorithm);
	return ((jlong)logger);
}

JNIEXPORT jstring JNICALL Java_de_libalf_jni_JNIBufferedLogger_receive_1and_1flush (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the logger object
	buffered_logger *logger = (buffered_logger*)pointer;

	// Get string
	string* str;
	str = logger->receive_and_flush();

	//Convert string
	const char* c = str->c_str();

	return env->NewStringUTF(c);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIBufferedLogger_destroy (JNIEnv *env, jobject obj, jlong pointer) {
	// Kill the learning algorithm
	delete (buffered_logger*)pointer;
}
