/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti */

#ifndef _Included_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
#define _Included_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    identifyImp
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_identifyImp
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    preprocessImp
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_preprocessImp
  (JNIEnv *, jobject, jlong, jstring, jstring);

/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    addSample
 * Signature: (Ljava/lang/String;Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_addSample
  (JNIEnv *, jobject, jstring, jstring, jboolean);

/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    trainingImp
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_trainingImp
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
