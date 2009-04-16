/* $Id$
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

#include <libalf/automaton_constructor.h>
#include <libalf/knowledgebase.h>
#include <libalf/learning_algorithm.h>
#include <libalf/algorithm_angluin.h>

#include <jni.h>

#include "jni_algorithm_angluin.h"

using namespace std;
using namespace libalf;

basic_automaton_holder* create_example() {
	basic_automaton_holder * a = new basic_automaton_holder;

	a->is_dfa = true;
	a->alphabet_size = 2;
	a->state_count = 3;

	a->start.insert(0);
	a->start.insert(1);
	a->final.insert(0);

	transition tr;

	// 0 -0-> 1
	tr.source=0;
	tr.label=0;
	tr.destination=1;
	a->transitions.insert(tr);

	// 0 -1-> 2
	tr.label=1;
	tr.destination=2;
	a->transitions.insert(tr);

	// 2 -1-> 1
	tr.source=2;
	tr.destination=1;
	a->transitions.insert(tr);

	// 2 -0-> 0;
	tr.label = 0;
	tr.destination = 0;
	a->transitions.insert(tr);

	// 1 -0-> 2;
	tr.source = 1;
	tr.destination = 2;
	a->transitions.insert(tr);

	// 1 -1-> 0;
	tr.label = 1;
	tr.destination = 0;
	a->transitions.insert(tr);

	return a;
}

jobject create_transition(JNIEnv* env, int source, int label, int destination) {
	/*
	 *Create new Java LibALFTransition object
	 */
	// Find class
	jclass jcls = env->FindClass("de/libalf/jni/BasicTransition");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'BasicTransition'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "(III)V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'BasicTransition'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_transition = env->NewObject(jcls, jmid, source, label, destination);
	if(jmid == NULL) {
		cout << "Could not create new 'BasicTransition' object!\nReturning NULL\n";
		return NULL;
	}
	return java_transition;
}

jobject convertAutomaton(JNIEnv* env, basic_automaton_holder* automaton) {
	/*
	 *Create new Java LibALFAutomaton object
	 */
	// Find class
	jclass jcls = env->FindClass("de/libalf/jni/BasicAutomaton");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'BaiscAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "(ZII)V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_automaton = env->NewObject(jcls, jmid, automaton->is_dfa, automaton->state_count, automaton->alphabet_size);
	if(jmid == NULL) {
		cout << "Could not create new 'BasicAutomaton' object!\nReturning NULL\n";
		return NULL;
	}

	/*
	 * Copy given automaton, i.e. its initial and final states as well as its transitions.
	 *
	 * First, process the initial states.
	 */
	// Find the add initial states method
	jmid = env->GetMethodID(jcls, "addInitialState", "(I)V");
	if(jmid == 0) {
		cout << "Could not find addInitialState of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Process all initial states
	set<int>::iterator i;
	for(i = automaton->start.begin(); i != automaton->start.end(); i++)
		// Add state to Java object
		env->CallVoidMethod(java_automaton, jmid, *i);

	/*
	 * Now, process the final states
	 */
	// Find the add final states method
	jmid = env->GetMethodID(jcls, "addFinalState", "(I)V");
	if(jmid == 0) {
		cout << "Could not find addFinalState of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Process all initial states
	for(i = automaton->final.begin(); i != automaton->final.end(); i++)
		// Add state to Java object
		env->CallVoidMethod(java_automaton, jmid, *i);

	/*
	 * Finally, process the transitions
	 */
	// Find the add transition method
	jmid = env->GetMethodID(jcls, "addTransition", "(Lde/libalf/jni/BasicTransition;)V");
	if(jmid == 0) {
		cout << "Could not find addTransition of 'BasicFAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Process all transitions
	transition_set::iterator si;
	for(si = automaton->transitions.begin(); si != automaton->transitions.end(); si++) {
		// Add state to Java object
		jobject tr = create_transition(env, si->source, si->label, si->destination);
		env->CallVoidMethod(java_automaton, jmid, tr);
	}

	return java_automaton;
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_AlgorithmAngluin_init (JNIEnv *env, jobject obj, jint alphabet_size, jint knowledgebase_pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new angluin_simple_observationtable<bool>(base, NULL, alphabet_size);
	return ((jint)algorithm);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_AlgorithmAngluin_add_1counterexample (JNIEnv *env , jobject obj, jintArray counterexample, jint pointer) {
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

JNIEXPORT jobject JNICALL Java_de_libalf_jni_AlgorithmAngluin_advance (JNIEnv *env, jobject obj, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Create a new automaton
	basic_automaton_holder* automaton = new basic_automaton_holder;

	// Advance!
	bool conjecture_ready = algorithm->advance(automaton);

	// Return a conjectrue if ready of NULL otherwise
	if(conjecture_ready == true) return convertAutomaton(env, automaton);
	else return NULL;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_AlgorithmAngluin_conjecture_1ready (JNIEnv *evn, jobject obj, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->conjecture_ready();
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_AlgorithmAngluin_get_1alphabet_1size (JNIEnv *env, jobject obj, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->get_alphabet_size();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_AlgorithmAngluin_increase_1alphabet_1size (JNIEnv *env, jobject obj, jint newSize, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->increase_alphabet_size(newSize);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_AlgorithmAngluin_set_1alphabet_1size (JNIEnv *env, jobject obj, jint newSize, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->set_alphabet_size(newSize);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_AlgorithmAngluin_set_1knowledge_1source (JNIEnv *env, jobject obj, jint knowledgebase_pointer, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) knowledgebase_pointer;
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->set_knowledge_source(base);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_AlgorithmAngluin_set_1knowledge_1source_1NULL (JNIEnv *env, jobject obj, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->set_knowledge_source(NULL);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_AlgorithmAngluin_sync_1to_1knowledgebase (JNIEnv *env, jobject obj, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->sync_to_knowledgebase();
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_AlgorithmAngluin_supports_1sync (JNIEnv *env, jobject obj, jint pointer) {
	// Get the algorithm object
	learning_algorithm<bool>* algorithm = (learning_algorithm<bool>*)pointer;

	// Forward method call
	return algorithm->supports_sync();
}

