/* $Id: jni_algorithm_nlstar.cpp 501 2009-04-20 15:38:54Z neider $
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
 * (c) 2009 by Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 */

#include <iostream>

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>
#include <libalf/logger.h>
#include <libalf/algorithm_NLstar.h>

#include <jni.h>

#include "jni_algorithm_nlstar.h"

using namespace std;
using namespace libalf;

JNIEXPORT jlong JNICALL Java_de_libalf_jni_NLstar_init__JI (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new NLstar_table<bool>(base, NULL, alphabet_size);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_NLstar_init__JIJ (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jint alphabet_size, jlong logger_pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) logger_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new NLstar_table<bool>(base, logger, alphabet_size);
	return ((jlong)algorithm);
}

