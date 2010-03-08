/* $Id$
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

#include <set>
#include <map>
#include <iostream>
#include <string>

#include "jni_tools.h"

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>
#include <libalf/normalizer.h>
#include <libalf/normalizer_msc.h>

#include <jni.h>

#include "jni_learning_algorithm.h"

using namespace std;
using namespace libalf;

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_add_1counterexample (JNIEnv *env , jobject obj, jintArray counterexample, jlong pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(counterexample);
	jint *entry = env->GetIntArrayElements(counterexample, 0);
	// Copy array
	int len = (int)length;
	list<int> ce;
	for(int i=0; i<len; i++) ce.push_back(((jint)entry[i]));
	// Clean
	env->ReleaseIntArrayElements(counterexample, entry, 0);

	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;
	// Forward method call
	algorithm->add_counterexample(ce);
}

JNIEXPORT jobject JNICALL Java_de_libalf_jni_JNILearningAlgorithm_advance (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Create a new automaton
	conjecture * cj;

	bool is_dfa;
	int alphabet_size, state_count;
	set<int> initial, final;
	multimap<pair<int, int>, int> transitions;

	// Advance!
	cj = algorithm->advance();

	if(cj != NULL) {
		// Return a conjectrue if ready
		jobject aut;
		simple_automaton * sa = dynamic_cast<simple_automaton*>(cj);
		if(sa == NULL) {
			fprintf(stderr, "FIXME: HYPOTHESIS IS NOT A SIMPLE AUTOMATON!\n");
			delete cj;
			return NULL;
		}

		aut = convertAutomaton(env, sa->is_deterministic, sa->alphabet_size, sa->state_count, sa->initial, sa->final, sa->transitions);

		delete cj;
		return aut;
	} else {
		return NULL;
	}
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNILearningAlgorithm_conjecture_1ready (JNIEnv *evn, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->conjecture_ready();
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNILearningAlgorithm_get_1alphabet_1size (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->get_alphabet_size();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_increase_1alphabet_1size (JNIEnv *env, jobject obj, jint newSize, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->increase_alphabet_size(newSize);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_set_1alphabet_1size (JNIEnv *env, jobject obj, jint newSize, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->set_alphabet_size(newSize);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_set_1knowledge_1source (JNIEnv *env, jobject obj, jlong knowledgebase_pointer, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->set_knowledge_source(base);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_set_1knowledge_1source_1NULL (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->set_knowledge_source(NULL);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNILearningAlgorithm_sync_1to_1knowledgebase (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->sync_to_knowledgebase();
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNILearningAlgorithm_supports_1sync (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->supports_sync();
}

JNIEXPORT jintArray JNICALL Java_de_libalf_jni_JNILearningAlgorithm_serialize (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Convert
	jintArray arr = basic_string2jintArray_tohl(env, algorithm->serialize());

	return arr;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNILearningAlgorithm_deserialize (JNIEnv *env, jobject obj, jintArray serialization, jlong pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(serialization);
	jint *entry = env->GetIntArrayElements(serialization, 0);
	// Copy array
	int len = (int)length;
	basic_string<int32_t> serial;
	for(int i=0; i<len; i++) serial.push_back(htonl((jint)entry[i]));
	// Clean
	env->ReleaseIntArrayElements(serialization, entry, 0);

	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	serial_stretch ser(serial);
	return algorithm->deserialize(ser);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_set_1logger (JNIEnv *env, jobject obj, jlong logger_pointer, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Get the logger object
	buffered_logger* logger = (buffered_logger*)logger_pointer;

	// Forward method call
	algorithm->set_logger(logger);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_destroy (JNIEnv *env, jobject obj, jlong pointer) {
	// Kill the learning algorithm
	delete (learning_algorithm<bool>*)pointer;
}

JNIEXPORT jstring JNICALL Java_de_libalf_jni_JNILearningAlgorithm_tostring (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Get string
	string str;
	str = algorithm->to_string();

	//Convert string
	const char* c = str.c_str();

	return env->NewStringUTF(c);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_set_1normalizer (JNIEnv *env, jobject obj, jlong normalizer_pointer, jlong pointer) {
	// Get the normalizer object
	normalizer_msc* norm = (normalizer_msc*)normalizer_pointer;
	
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;
	
	// Forward method call
	algorithm->set_normalizer(norm);
}
  
JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_set_1normalizer_1NULL (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;
	
	// Forward method call
	algorithm->set_normalizer(NULL);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_remove_1normalizer (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;
	
	// Forward method call
	algorithm->unset_normalizer();
}
