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
#include <libalf/algorithm_kearns_vazirani.h>

#include <jni.h>

#include "jni_algorithm_kearns_vazirani.h"

using namespace std;
using namespace libalf;

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_init__JI (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new kearns_vazirani<bool>(base, NULL, alphabet_size, true);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_init__JIJ (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size, jlong logger_pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) logger_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new kearns_vazirani<bool>(base, logger, alphabet_size, true);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_init__JIZ (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size, jboolean use_binary_search) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;
	
	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new kearns_vazirani<bool>(base, NULL, alphabet_size, use_binary_search);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_init__JIJZ (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size, jlong logger_pointer, jboolean use_binary_search) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) logger_pointer;
	
	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new kearns_vazirani<bool>(base, logger, alphabet_size, use_binary_search);
	return ((jlong)algorithm);
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_get_1inner_1node_1count (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	kearns_vazirani<bool>* algorithm = (kearns_vazirani<bool>*)pointer;

	// Forward method call
	return algorithm->get_inner_node_count();
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_get_1leaf_1node_1count (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	kearns_vazirani<bool>* algorithm = (kearns_vazirani<bool>*)pointer;

	// Forward method call
	return algorithm->get_leaf_node_count();
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_uses_1binary_1search (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	kearns_vazirani<bool>* algorithm = (kearns_vazirani<bool>*)pointer;

	// Forward method call
	return algorithm->uses_binary_search();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIAlgorithmKearnsVazirani_set_1binary_1search (JNIEnv *env, jobject obj, jlong pointer, jboolean use_binary_search) {
	// Get the algorithm object
	kearns_vazirani<bool>* algorithm = (kearns_vazirani<bool>*)pointer;

	// Forward method call
	return algorithm->set_binary_search(use_binary_search);
}

