#include <jni.h>
#include "com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti.h"
#include "VeriCodeIdenti.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    identifyImp
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_identifyImp
  (JNIEnv * env, jobject obj, jlong mat)
  {
      VeriCodeIdenti& instance = VeriCodeIdenti::getInstance();
      Mat& myMat = *(Mat*)mat;
      string code = instance.identify(myMat);
      return env->NewStringUTF(code.c_str());
  }

/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    preprocessImp
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_preprocessImp
  (JNIEnv *env, jobject obj, jlong mat, jstring savePath, jstring namePrefix)
  {
      Mat& myMat =*(Mat*)mat;
      const char* sp = (const char*)env->GetStringUTFChars(savePath, NULL);
      string mySavepath(sp);
      env->ReleaseStringUTFChars(savePath, sp);
      const char* np = (const char*)env->GetStringUTFChars(namePrefix, NULL);
      string myNamePrefix(np);
      env->ReleaseStringUTFChars(namePrefix, np);
      VeriCodeIdenti &instance = VeriCodeIdenti::getInstance();
      instance.preprocess(myMat, mySavepath, myNamePrefix);
      return;
  }

/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    addSample
 * Signature: (Ljava/lang/String;Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_addSample
  (JNIEnv * env, jobject obj, jstring fullPaht, jstring label, jboolean clearPrev)
  {
      const char* fp = (const char*)env->GetStringUTFChars(fullPaht, NULL);
      string myFullPath(fp);
      env->ReleaseStringUTFChars(fullPaht, fp);
      const char* lb  = (const char*)env->GetStringUTFChars(label, NULL);
      string myLabel(lb);
      env->ReleaseStringUTFChars(label,lb); 
      VeriCodeIdenti& instance = VeriCodeIdenti::getInstance();
      instance.addSample(myFullPath, myLabel);
  }

/*
 * Class:     com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti
 * Method:    trainingImp
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_github_xxhvictor_VeriCodeIdenti_VeriCodeIdenti_trainingImp
  (JNIEnv *, jobject)
  {
      VeriCodeIdenti& instance = VeriCodeIdenti::getInstance();
      instance.training();
  }

#ifdef __cplusplus
}
#endif

