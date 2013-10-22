/* $Id: jni_algorithm_angluin.cpp 1081 2009-11-11 16:33:40Z davidpiegdon $
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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: Daniel Neider <neider@automata.rwth-aachen.de>
 *
 */

#include <iostream>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>
#include <libalf/algorithm_deterministic_inferring_z3.h>

#include <jni.h>

#include "jni_algorithm_deterministic_inferring_z3.h"

using namespace std;
using namespace libalf;


JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_init__JJIZZ (JNIEnv * env, jobject obj, jlong kb_pointer, jlong log_pointer, jint alphabet_size, jboolean use_variables, jboolean use_enum) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) log_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new deterministic_inferring_Z3<bool>(base, logger, alphabet_size, use_variables, use_enum);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_init__JIZZ (JNIEnv * env, jobject obj, jlong kb_pointer, jint alphabet_size, jboolean use_variables, jboolean use_enum) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new deterministic_inferring_Z3<bool>(base, NULL, alphabet_size, use_variables, use_enum);
	return ((jlong)algorithm);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_is_1using_1variables (JNIEnv * env, jobject obj, jlong pointer) {
	// Get the algorithm object
	deterministic_inferring_Z3<bool>* algorithm = (deterministic_inferring_Z3<bool>*)pointer;

	// Forward method call
	return algorithm->is_using_variables();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_set_1using_1variables (JNIEnv * env, jobject obj, jlong pointer, jboolean using_variables) {
	// Get the algorithm object
	deterministic_inferring_Z3<bool>* algorithm = (deterministic_inferring_Z3<bool>*)pointer;

	// Forward method call
	algorithm->set_using_variables(using_variables);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_is_1using_1enum (JNIEnv * env, jobject obj, jlong pointer) {
	// Get the algorithm object
	deterministic_inferring_Z3<bool>* algorithm = (deterministic_inferring_Z3<bool>*)pointer;

	// Forward method call
	return algorithm->is_using_enum();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_set_1using_1enum (JNIEnv * env, jobject obj, jlong pointer, jboolean using_enum) {
	// Get the algorithm object
	deterministic_inferring_Z3<bool>* algorithm = (deterministic_inferring_Z3<bool>*)pointer;

	// Forward method call
	algorithm->set_using_enum(using_enum);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_is_1logging_1model (JNIEnv * env, jobject obj, jlong pointer) {
	// Get the algorithm object
	deterministic_inferring_Z3<bool>* algorithm = (deterministic_inferring_Z3<bool>*)pointer;

	// Forward method call
	return algorithm->is_logging_model();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringZ3_set_1log_1model (JNIEnv * env, jobject obj, jlong pointer, jboolean log_model) {
	// Get the algorithm object
	deterministic_inferring_Z3<bool>* algorithm = (deterministic_inferring_Z3<bool>*)pointer;

	// Forward method call
	algorithm->set_log_model(log_model);
}
