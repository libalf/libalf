/* $Id: jni_tools.cpp 814 2009-09-04 08:28:54Z stefanschulz $
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

#include "jni_tools.h"
#include <iostream>

#include <jni.h>

using namespace std;

jintArray basic_string2jintArray(JNIEnv *env, basic_string<int32_t> str)
{
	// Create new Java int array
	int strSize = str.size();
	jintArray arr = env->NewIntArray(strSize);

	// Copy array
	int intArray[strSize];
	int i=0;
	basic_string<int32_t>::iterator si;
	for(si = str.begin(); si != str.end(); si++) {
		intArray[i] = *si;
		i++;
	}

	// Fill Java array
	env->SetIntArrayRegion(arr, 0, strSize, (jint *)intArray);

	return arr;
}

jintArray list_int2jintArray(JNIEnv *env, list<int> l) {
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

jobject create_transition(JNIEnv* env, int source, int label, int destination) {
	/*
	 *Create new Java LibALFTransition object
	 */
	// Find class
	jclass jcls = env->FindClass("de/libalf/BasicTransition");
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

jobject convertAutomaton(JNIEnv* env, bool is_dfa, int alphabet_size, int state_count, set<int> & initial, set<int> & final, multimap<pair<int, int>, int> & transitions) {
	/*
	 *Create new Java LibALFAutomaton object
	 */
	// Find class
	jclass jcls = env->FindClass("de/libalf/BasicAutomaton");
	if(jcls == NULL) {
		cout << "Could not find Java Class 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Find constructor
	jmethodID jmid = env->GetMethodID(jcls, "<init>", "(ZII)V");
	if(jmid == NULL) {
		cout << "Could not find constructor of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Make new object
	jobject java_automaton = env->NewObject(jcls, jmid, is_dfa, state_count, alphabet_size);
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
	for(i = initial.begin(); i != initial.end(); i++)
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
	// Process all final states
	for(i = final.begin(); i != final.end(); i++)
		// Add state to Java object
		env->CallVoidMethod(java_automaton, jmid, *i);

	/*
	 * Finally, process the transitions
	 */
	// Find the add transition method
	jmid = env->GetMethodID(jcls, "addTransition", "(Lde/libalf/BasicTransition;)V");
	if(jmid == 0) {
		cout << "Could not find addTransition of 'BasicAutomaton'!\nReturning NULL\n";
		return NULL;
	}
	// Process all transitions
	multimap<pair<int, int>, int>::iterator si;
	for(si = transitions.begin(); si != transitions.end(); si++) {
		// Add state to Java object
		jobject tr = create_transition(env, si->first.first, si->first.second, si->second);
		env->CallVoidMethod(java_automaton, jmid, tr);
	}

	return java_automaton;
}
