/* $Id: jni_algorithm_angluin.cpp 490 2009-04-16 20:40:34Z davidpiegdon $
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) Daniel Neider, i7 Informatik RWTH-Aachen
 *     <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */

#include <set>
#include <map>
#include <iostream>
#include <string>

#include "jni_tools.h"

#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>

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
	bool is_dfa;
	int alphabet_size, state_count;
	set<int> initial, final;
	multimap<pair<int, int>, int> transitions;

	// Advance!
	bool conjecture_ready = algorithm->advance(is_dfa, alphabet_size, state_count, initial, final, transitions);

	// Return a conjectrue if ready of NULL otherwise
	if(conjecture_ready == true) return convertAutomaton(env, is_dfa, alphabet_size, state_count, initial, final, transitions);
	else return NULL;
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
	jintArray arr = basic_string2jintArray(env, algorithm->serialize());

	return arr;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNILearningAlgorithm_deserialize (JNIEnv *env, jobject obj, jintArray serialization, jlong pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(serialization);
	jint *entry = env->GetIntArrayElements(serialization, 0);
	// Copy array
	int len = (int)length;
	basic_string<int32_t> ser;
	for(int i=0; i<len; i++) ser.push_back(((jint)entry[i]));
	// Clean
	env->ReleaseIntArrayElements(serialization, entry, 0);

	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	basic_string<int32_t>::iterator si;
	si = ser.begin();
	return algorithm->deserialize(si, ser.end());
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_set_1logger (JNIEnv *env, jobject obj, jlong logger_pointer, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Get the logger object
	buffered_logger* logger = (buffered_logger*)logger_pointer;

	// Forward method call
	algorithm->set_logger(logger);
}

JNIEXPORT jstring JNICALL Java_de_libalf_jni_JNILearningAlgorithm_tostring (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Get string
	string str;
	str = algorithm->tostring();

	//Convert string
	const char* c = str.c_str();

	return env->NewStringUTF(c);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNILearningAlgorithm_destroy (JNIEnv *env, jobject obj, jlong pointer) {
	// Kill the learning algorithm
	delete (learning_algorithm<bool>*)pointer;
}