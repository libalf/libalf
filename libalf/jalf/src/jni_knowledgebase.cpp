/* $Id: jni_knowledgebase.cpp 564 2009-04-30 15:11:18Z davidpiegdon $
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
#include <string>

#include "jni_tools.h"

#include <libalf/knowledgebase.h>

#include <jni.h>

#include "jni_knowledgebase.h"

using namespace std;
using namespace libalf;

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIKnowledgebase_init (JNIEnv *env, jobject obj) {
	/*
	 * Return the new object
	 */
	knowledgebase<bool> *base = new knowledgebase<bool>;
	return ((jlong)base);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIKnowledgebase_is_1answered (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->is_empty();
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIKnowledgebase_is_1empty (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->is_answered();
}

JNIEXPORT jobject JNICALL Java_de_libalf_jni_JNIKnowledgebase_get_1knowledge (JNIEnv *env, jobject obj, jlong pointer) {
	/*
	 *Create new Java LinkedList object
	 */
	// Find class
	jclass jcls = env->FindClass("java/util/LinkedList");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'LinkedList'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "()V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'LinkedList'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_queries = env->NewObject(jcls, jmid);
	if(jmid == NULL) {
		cout << "Could not create new 'LinkedList' object!\nReturning NULL\n";
		return NULL;
	}

	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	/*
	 * Fill list
	 */
	knowledgebase<bool>::iterator ki;

	// Find add method
	jmid = env->GetMethodID(jcls, "add", "(Ljava/lang/Object;)Z");
	if(jmid == NULL) {
		cout << "Could not find add method of 'LinkedList'!\nReturning NULL\n";
		return NULL;
	}
	// Loop through all queries
	for(ki = base->begin(); ki != base->end(); ki++) {
		// Convert list to a Java array
		jintArray arr = list_int2jintArray(env, ki->get_word());

		/*
		 * Add the array to the linked list
		 */
		env->CallBooleanMethod(java_queries, jmid, arr);

	}

	return java_queries;
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNIKnowledgebase_count_1queries (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->count_queries();
}

JNIEXPORT jobject JNICALL Java_de_libalf_jni_JNIKnowledgebase_get_1queries (JNIEnv *env, jobject obj, jlong pointer) {
	/*
	 *Create new Java LinkedList object
	 */
	// Find class
	jclass jcls = env->FindClass("java/util/LinkedList");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'LinkedList'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "()V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'LinkedList'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_queries = env->NewObject(jcls, jmid);
	if(jmid == NULL) {
		cout << "Could not create new 'LinkedList' object!\nReturning NULL\n";
		return NULL;
	}

	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	/*
	 * Fill list
	 */
	knowledgebase<bool>::iterator ki;

	// Find add method
	jmid = env->GetMethodID(jcls, "add", "(Ljava/lang/Object;)Z");
	if(jmid == NULL) {
		cout << "Could not find add method of 'LinkedList'!\nReturning NULL\n";
		return NULL;
	}
	// Loop through all queries
	for(ki = base->qbegin(); ki != base->qend(); ki++) {
		// Convert list to a Java array
		jintArray arr = list_int2jintArray(env, ki->get_word());

		/*
		 * Add the array to the linked list
		 */
		env->CallBooleanMethod(java_queries, jmid, arr);

	}

	return java_queries;
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNIKnowledgebase_resolve_1query (JNIEnv *env , jobject obj, jintArray word, jlong pointer) {
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
	bool acc;
	bool exists = base->resolve_query(w, acc);
	
	if(exists) {
		if(acc) return 2;
		else return 0;
	}
	else return 1;
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNIKnowledgebase_resolve_1or_1add_1query (JNIEnv *env, jobject obj, jintArray word, jlong pointer) {
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
	bool acc;
	bool exists = base->resolve_or_add_query(w, acc);
	
	if(exists) {
		if(acc) return 2;
		else return 0;
	}
	else return 1;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIKnowledgebase_add_1knowledge (JNIEnv *env, jobject obj, jintArray word, jboolean acceptance, jlong pointer) {
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

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIKnowledgebase_clear (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	base->clear();
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIKnowledgebase_clear_1queries (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	base->clear_queries();
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIKnowledgebase_undo (JNIEnv *env, jobject obj, jint count , jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	base->undo(count);
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNIKnowledgebase_get_1memory_1usage (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->get_memory_usage();
}

JNIEXPORT jint JNICALL Java_de_libalf_jni_JNIKnowledgebase_count_1answers (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	return base->count_answers();
}

JNIEXPORT jstring JNICALL Java_de_libalf_jni_JNIKnowledgebase_generate_1dotfile (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Get string
	string str;
	str = base->generate_dotfile();

	//Convert string
	const char* c = str.c_str();

	return env->NewStringUTF(c);
}

JNIEXPORT jintArray JNICALL Java_de_libalf_jni_JNIKnowledgebase_serialize (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Convert
	jintArray arr = basic_string2jintArray(env, base->serialize());

	return arr;
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIKnowledgebase_deserialize (JNIEnv *env, jobject obj, jintArray serialization, jlong pointer) {
	// Get Java array info
	jsize length = env->GetArrayLength(serialization);
	jint *entry = env->GetIntArrayElements(serialization, 0);
	// Copy array
	int len = (int)length;
	basic_string<int32_t> ser;
	for(int i=0; i<len; i++) ser.push_back(((jint)entry[i]));
	// Clean
	env->ReleaseIntArrayElements(serialization, entry, 0);

	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Forward method call
	basic_string<int32_t>::iterator si;
	si = ser.begin();
	return base->deserialize(si, ser.end());
}

JNIEXPORT jstring JNICALL Java_de_libalf_jni_JNIKnowledgebase_tostring (JNIEnv *env , jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*)pointer;

	// Get string
	string str;
	str = base->tostring();

	//Convert string
	const char* c = str.c_str();

	return env->NewStringUTF(c);
}

JNIEXPORT void JNICALL Java_de_libalf_jni_JNIKnowledgebase_destroy (JNIEnv *env, jobject obj, jlong pointer) {
	// Kill the learning algorithm
	delete (knowledgebase<bool>*)pointer;
}
