/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class de_libalf_jni_Knowledgebase */

#ifndef _Included_de_libalf_jni_Knowledgebase
#define _Included_de_libalf_jni_Knowledgebase
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    init
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_de_libalf_jni_Knowledgebase_init
  (JNIEnv *, jobject);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    is_answered
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_is_1answered
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    getKnowledge
 * Signature: (J)Lde/libalf/jni/WordList;
 */
JNIEXPORT jobject JNICALL Java_de_libalf_jni_Knowledgebase_getKnowledge
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    is_empty
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_is_1empty
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    count_queries
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_count_1queries
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    getQueries
 * Signature: (J)Lde/libalf/jni/WordList;
 */
JNIEXPORT jobject JNICALL Java_de_libalf_jni_Knowledgebase_getQueries
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    resolve_query
 * Signature: ([IJ)I
 */
JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_resolve_1query
  (JNIEnv *, jobject, jintArray, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    resolve_or_add_query
 * Signature: ([IJ)I
 */
JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_resolve_1or_1add_1query
  (JNIEnv *, jobject, jintArray, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    add_knowledge
 * Signature: ([IZJ)Z
 */
JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_add_1knowledge
  (JNIEnv *, jobject, jintArray, jboolean, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    clear
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_de_libalf_jni_Knowledgebase_clear
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    clear_queries
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_de_libalf_jni_Knowledgebase_clear_1queries
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    undo
 * Signature: (IJ)Z
 */
JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_undo
  (JNIEnv *, jobject, jint, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    get_memory_usage
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_get_1memory_1usage
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    count_answers
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_de_libalf_jni_Knowledgebase_count_1answers
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    generate_dotfile
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_de_libalf_jni_Knowledgebase_generate_1dotfile
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    serialize
 * Signature: (J)[I
 */
JNIEXPORT jintArray JNICALL Java_de_libalf_jni_Knowledgebase_serialize
  (JNIEnv *, jobject, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    deserialize
 * Signature: ([IJ)Z
 */
JNIEXPORT jboolean JNICALL Java_de_libalf_jni_Knowledgebase_deserialize
  (JNIEnv *, jobject, jintArray, jlong);

/*
 * Class:     de_libalf_jni_Knowledgebase
 * Method:    tostring
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_de_libalf_jni_Knowledgebase_tostring
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
