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

#include <string>
#include <list>

#include <jni.h>

using namespace std;

int32_t *basic_string2intarray(basic_string<int32_t> str)
{{{
	int32_t *res = new int32_t[str.size()];
	basic_string<int32_t>::iterator si;
	int i;

	for(i = 0, si = str.begin(); si != str.end(); i++, si++)
		res[i] = *si;

	return res;
}}}

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