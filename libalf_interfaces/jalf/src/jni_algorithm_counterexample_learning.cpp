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
#include <libalf/algorithm_counterexample_learning.h>

#include <jni.h>

#include "jni_algorithm_counterexample_learning.h"

using namespace std;
using namespace libalf;
JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmCounterexampleLearning_init__JJIJ (JNIEnv * env, jobject obj, jlong kb_pointer, jlong log_pointer, jint alphabet_size, jlong inferring_algorithm) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	// Get the logger object
	buffered_logger *logger = (buffered_logger*) log_pointer;

	// Get the inferring algorithm object
	inferring_algorithm<bool> * inferring_alg = (inferring_algorithm<bool>*) inferring_algorithm;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new algorithm_counterexample_learning(base, logger, alphabet_size, inferring_alg);
	return ((jlong)algorithm);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmCounterexampleLearning_init__JIJ (JNIEnv * env, jobject obj, jlong kb_pointer, jlong log_pointer, jint alphabet_size, jlong inferring_algorithm) {
	// Get the knowledgebase object
	knowledgebase<bool> *base = (knowledgebase<bool>*) kb_pointer;

	// Get the inferring algorithm object
	inferring_algorithm<bool> * inferring_alg = (inferring_algorithm<bool>*) inferring_algorithm;

	/*
	 * Return the new object
	 */
	learning_algorithm<bool>* algorithm = new algorithm_counterexample_learning(base, NULL, alphabet_size, inferring_alg);
	return ((jlong)algorithm);
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_JNIAlgorithmCounterexampleLearning_add_1counterexample (JNIEnv * env, jobject obj, jlong pointer, jobject word) {
	// Get the algorithm object
	algorithm_counterexample_learning* algorithm = (algorithm_counterexample_learning*)pointer;

	// Decoding word from Java:List<Integer> to C++:list<int>
	std::list<int> list_word;
	jclass clsList = env->FindClass("java/util/List");
	jmethodID midListSize = 0;
	jmethodID midListGet = 0;
	if(clsList != 0){
		midListSize = env->GetMethodID(clsList, "size", "()I");
		midListGet = env->GetMethodID(clsList, "get", "(I)Ljava/lang/Object;");
	}
	jclass clsInteger = env->FindClass("java/lang/Integer");
	jmethodID midIntegerIntValue = 0;
	if(clsInteger != 0){
		midIntegerIntValue = env->GetMethodID(clsInteger, "intValue", "()I");
	}

	if((clsList != 0) && (midListSize != 0)){
		int list_size = env->CallIntMethod(word,  midListSize );
		if((list_size > 0) && (midListGet != 0) && (clsInteger != 0) && (midIntegerIntValue != 0)){

			for(int i=0; i < list_size; i++){
				jobject listElement = env->CallObjectMethod(word, midListGet, i);
				int value = env->CallIntMethod(listElement, midIntegerIntValue);                              
				list_word.push_back(value);
			}      
		}
	} 

	// Forward method call
	return algorithm->add_counter_example(list_word);
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_JNIAlgorithmCounterexampleLearning_derive_1conjecture (JNIEnv * env, jobject obj, jlong pointer) {
	// Get the algorithm object
	algorithm_counterexample_learning* algorithm = (algorithm_counterexample_learning*)pointer;

	// Forward method call
	return ((jlong)(algorithm->derive_conjecture()));
}
