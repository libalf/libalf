/* $Id$
 * vim: fdm=marker
 *
 * libalf JNI - Java Native Interface for Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *    and Daniel Neider, i7 Informatik RWTH-Aachen
 *        <neider@automata.rwth-aachen.de>
 *
 * see LICENSE file for licensing information.
 */

#include <iostream>

#include <libalf/automaton_constructor.h>
#include <libalf/knowledgebase.h>

#include <jni.h>

#include "jni_knowledgebase.h"

using namespace std;
using namespace libalf;

jintArray makeJavaIntArray(JNIEnv *env, list<int> l) {
	// Create new Java int array
	jintArray arr = env->NewIntArray(l.size());

	// Copy array
	int intArray[l.size()];
	int i=0;
	list<int>::iterator li;
	for(li = l.begin(); li != l.end(); li++) {
		intArray[i] = *li;
		i++;
	}

	// Fill Java array
	env->SetIntArrayRegion(arr, 0, l.size(), (jint *)intArray);

	return arr;
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_init (JNIEnv *env, jobject obj) {
	/*
	 * Return the new object
	 */
	knowledgebase<bool> *base = new knowledgebase<bool>;
	return ((jint)base);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_is_1answered (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->is_empty();
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_is_1empty (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->is_answered();
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_count_1queries (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->count_queries();
}

JNIEXPORT jobject JNICALL Java_de_libalf_jni_Knowledgebase_getQueries (JNIEnv *env, jobject obj, jint pointer) {
	/*
	 *Create new Java LinkedList object
	 */
	// Find class
	jclass jcls = env->FindClass("de/libalf/jni/Queries");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'Queries'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "()V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'Queries'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_queries = env->NewObject(jcls, jmid);
	if(jmid == NULL) {
		cout << "Could not create new 'Queries' object!\nReturning NULL\n";
		return NULL;
	}

	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	/*
	 * Fill list
	 */
	knowledgebase<bool>::iterator ki;

	// Find add method
	//jmid = env->GetMethodID(jcls, "add", "(Ljava/lang/Object;)Z ");
	jmid = env->GetMethodID(jcls, "jniAdd", "([I)V");
	if(jmid == NULL) {
		cout << "Could not find add method of 'LinkedList'!\nReturning NULL\n";
		return NULL;
	}
	// Loop through all queries
	for(ki = base->qbegin(); ki != base->qend(); ki++) {
		// Convert list to a Java array
		jintArray arr = makeJavaIntArray(env, ki->get_word());

		/*
		 * Add the array to the linked list
		 */
		env->CallVoidMethod(java_queries, jmid, arr);

	}

	return java_queries;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_resolve_1query (JNIEnv *env, jobject obj, jintArray word, jboolean acceptance, jint pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(word);
	jint *entry = env->GetIntArrayElements(word, 0);
	// Copy array
	int len = (int)length;
	list<int> w;
	for(int i=0; i<len; i++) w.push_back(((jint)entry[i]));
	// Clean
	env->ReleaseIntArrayElements(word, entry, 0);

	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;
	// Forward method call
	bool acc = acceptance;
	return base->resolve_query(w, acc);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_add_1knowledge (JNIEnv *env, jobject obj, jintArray word, jboolean acceptance, jint pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(word);
	jint *entry = env->GetIntArrayElements(word, 0);
	// Copy array
	int len = (int)length;
	list<int> w;
	for(int i=0; i<len; i++) w.push_back(((jint)entry[i]));
	// Clean
	env->ReleaseIntArrayElements(word, entry, 0);

	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;
	// Forward method call
	bool acc = acceptance;
	return base->add_knowledge(w, acc);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_Knowledgebase_clear (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	base->clear();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_Knowledgebase_clear_1queries (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	base->clear_queries();
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_undo (JNIEnv *env, jobject obj, jint count , jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	base->undo(count);
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_get_1memory_1usage (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->get_memory_usage();
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_get_1timestamp (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->get_timestamp();
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_count_1answers (JNIEnv *env, jobject obj, jint pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->count_answers();
}