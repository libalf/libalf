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
#include <libalf/algorithm_deterministic_inferring_csp_minisat.h>

#include <jni.h>

#include "jni_algorithm_deterministic_inferring_csp_minisat.h"

using namespace std;
using namespace libalf;
JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_init__JJI (JNIEnv * env, jobject obj, jlong kb_pointer, jlong log_pointer, jint alphabet_size) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) log_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new deterministic_inferring_csp_MiniSat<bool>(base, logger, alphabet_size, true);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_init__JI (JNIEnv * env, jobject obj, jlong kb_pointer, jint alphabet_size) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new deterministic_inferring_csp_MiniSat<bool>(base, NULL, alphabet_size, true);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_init__JJIZ (JNIEnv * env, jobject obj, jlong kb_pointer, jlong log_pointer, jint alphabet_size, jboolean use_binary_search) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) log_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new deterministic_inferring_csp_MiniSat<bool>(base, logger, alphabet_size, use_binary_search);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_init__JIZ (JNIEnv * env, jobject obj, jlong kb_pointer, jint alphabet_size, jboolean use_binary_search) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new deterministic_inferring_csp_MiniSat<bool>(base, NULL, alphabet_size, use_binary_search);
	return ((jlong)algorithm);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_uses_1unary_1encoding (JNIEnv * env, jobject obj, jlong pointer) {
	// Get the algorithm object
	deterministic_inferring_csp_MiniSat<bool>* algorithm = (deterministic_inferring_csp_MiniSat<bool>*)pointer;

	// Forward method call
	return algorithm->uses_unary_encoding();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_set_1unary_1encoding (JNIEnv * env, jobject obj, jlong pointer, jboolean unary_encoding) {
	// Get the algorithm object
	deterministic_inferring_csp_MiniSat<bool>* algorithm = (deterministic_inferring_csp_MiniSat<bool>*)pointer;

	// Forward method call
	algorithm->set_unary_encoding(unary_encoding);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_is_1logging_1model (JNIEnv * env, jobject obj, jlong pointer) {
	// Get the algorithm object
	deterministic_inferring_csp_MiniSat<bool>* algorithm = (deterministic_inferring_csp_MiniSat<bool>*)pointer;

	// Forward method call
	return algorithm->is_logging_model();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIAlgorithmDeterministicInferringCSPMinisat_set_1log_1model (JNIEnv * env, jobject obj, jlong pointer, jboolean log_model) {
	// Get the algorithm object
	deterministic_inferring_csp_MiniSat<bool>* algorithm = (deterministic_inferring_csp_MiniSat<bool>*)pointer;

	// Forward method call
	algorithm->set_log_model(log_model);
}
