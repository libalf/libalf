#include <iostream>

#include <list>
#include <libalf/knowledgebase.h>

#include "jni_query_iterator.h"

using namespace std;
using namespace libalf;

class query_iterator {
	private:
	knowledgebase<bool>* base;
	knowledgebase<bool>::iterator ki;

	public:
		query_iterator(knowledgebase<bool>* b) {
			base = b;
		}

		int* nextQuery(int &);
		void print(int*, int);
		bool answer(bool);
		void print(list<int>);
};

bool query_iterator::answer(bool acceptance) {
	return ki->set_answer(acceptance); 
}

int* query_iterator::nextQuery(int& size) {
	ki = base->qbegin();
	if(ki != base->qend()) {
	
		// Copy array
		list<int> l = ki->get_word();
		int* intArray;
		size = l.size();
		intArray = new int[size];
		int i=0;
		list<int>::iterator li;
		for(li = l.begin(); li != l.end(); li++) {
			intArray[i] = *li;
			i++;
		}
		return intArray;
	} else return NULL;
}

void query_iterator::print(int* arr, int size) {
	cout << "array contains: ";	
	for(int i=0; i<size; i++) cout << arr[i] << " ";
	cout << endl;
}

void query_iterator::print(list<int> mylist) {
	list<int>::iterator it;
	cout << "query_iterator::mylist contains:";
  	for ( it=mylist.begin() ; it != mylist.end(); it++ )
		cout << " " << *it;

	cout << endl;
}

JNIEXPORT jlong JNICALL Java_de_libalf_jni_QueryIterator_init (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the knowledgebase object
	knowledgebase<bool>* base = (knowledgebase<bool>*)pointer;

	/*
	 * Return the new object
	 */
	query_iterator *qi = new query_iterator(base);
	return ((jlong)qi);
}

JNIEXPORT jintArray JNICALL Java_de_libalf_jni_QueryIterator_nextQuery (JNIEnv *env, jobject obj, jlong pointer) {
	// Get the query_iterator object
	query_iterator* qi = (query_iterator*)pointer;

	// Get next query
	int size;
	int* query = qi->nextQuery(size);
	
	// Check whether return value in NULL
	if(query == NULL) return NULL;
	else {
		// Create new Java int array
		jintArray arr = env->NewIntArray(size);
		// Fill Java array
		env->SetIntArrayRegion(arr, 0, size, (jint *)query);

		return arr;
	}
}

JNIEXPORT jboolean JNICALL Java_de_libalf_jni_QueryIterator_answer (JNIEnv *env, jobject obj, jboolean acc, jlong pointer) {
	// Get the query_iterator object
	query_iterator* qi = (query_iterator*)pointer;

	// Forward method call
	return qi->answer(acc);
}

int main () {
	knowledgebase<bool> *base = new knowledgebase<bool>();

	list<int> myList;
	myList.push_front(3);
	myList.push_front(5);
	myList.push_front(2);
	myList.push_front(1);
	base->add_query(myList);
	
	list<int> myList2;
	myList2.push_front(2);
	myList2.push_front(1);
	base->add_query(myList2);

	query_iterator q(base);

	int* query;
	int size;
	cout << "\n";
	int i = 1;
	while((query = q.nextQuery(size)) != NULL) {
		cout << "query == null -> " << (query == NULL) << "\t";
		cout << i << "\t";
		q.print(query, size);
		i++;
		q.answer(true);
	}
	cout << endl;
	
	return 0;
}
