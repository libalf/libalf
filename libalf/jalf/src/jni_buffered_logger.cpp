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
 * (c) 2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

#include <iostream>

#include <libalf/logger.h>

#include <jni.h>

#include "jni_buffered_logger.h"

using namespace std;
using namespace libalf;

enum logger_loglevel map_jni_loggerLevel(int loglevel) {
	switch(loglevel) {
	case 1:
		return LOGGER_ERROR;
	case 2:
		return LOGGER_WARN;
	case 3:
		return LOGGER_INFO;
	case 4:
		return LOGGER_DEBUG;
	default:
		return LOGGER_ERROR;
	}
}

int map_jni_loggerLevel(enum logger_loglevel loglevel) {
	switch(loglevel) {
	case LOGGER_ERROR: 
		return 1;
	case LOGGER_WARN:
		return 2;
	case LOGGER_INFO:
		return 3;
	case LOGGER_DEBUG:
		return 4;
	default:
		return 1; 
	}
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIBufferedLogger_init__ (JNIEnv *env, jobject obj) {
	/*
	 * Return the new object
	 */
	buffered_logger* logger = new buffered_logger();
	return ((jlong)logger);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIBufferedLogger_init__IZ (JNIEnv *env, jobject obj, jint minimal_loglevel, jboolean log_algorithm) {
	// Select log level
	enum logger_loglevel loglevel =  map_jni_loggerLevel(minimal_loglevel);

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

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIBufferedLogger_set_1min_1loglevel (JNIEnv *env, jobject obj, jint logLevel, jlong pointer) {
	// Get the logger object
	buffered_logger *logger = (buffered_logger*)pointer;

	// Select log level
	enum logger_loglevel loglevel =  map_jni_loggerLevel(logLevel);

	// Forward method call
	logger->set_minimal_loglevel(loglevel);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIBufferedLogger_destroy (JNIEnv *env, jobject obj, jlong pointer) {
	// Kill the learning algorithm
	delete (buffered_logger*)pointer;
}
