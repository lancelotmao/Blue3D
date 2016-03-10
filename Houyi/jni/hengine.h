#ifndef HENGINE_H_
#define HENGINE_H_

#include <android/bitmap.h>
#include <unistd.h>

#include "Houyi.h"

#define JNI_METHOD(clz,type,name) JNIEXPORT type JNICALL Java_com_dwtech_android_houyi_##clz##_##name

#define HOUYI_OBJECT_CLASS_NAME "com/dwtech/android/houyi/HouyiObject"
#define HOUYI_OBJECT_ID_NAME    "mHouyiId"

extern Root* mRoot;
jfieldID g_nHouyiIdField = 0;

// init only once for each shared library
bool gInited = false;
pthread_mutex_t mJNIMutex;

// declared in android.h
JNIEnv* gJNIEnv;
JavaVM* gJVM;
jobject gContext;
AAssetManager* gAssetManager;
jobject displayMetrics;

class USE_JSTRING
{
private:
    JNIEnv * m_pEnv;
    jstring m_str;
    const char * m_pBuf;
public:
    USE_JSTRING(JNIEnv * env, jstring str) : m_str(str), m_pEnv(env) {m_pBuf = env->GetStringUTFChars(str, NULL);}
    ~USE_JSTRING()
    {
        m_pEnv->ReleaseStringUTFChars(m_str, m_pBuf);
        m_pEnv->DeleteLocalRef(m_str);
    }
    operator const char*() const
    {
        return m_pBuf;
    }
};

void * GetHouyiObject(JNIEnv * env, jobject obj);

#endif
