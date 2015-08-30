#include <android/bitmap.h>
#include "HouyiAndroidImage.h"
#include "HouyiLog.h"

namespace Houyi
{
    AndroidImage::AndroidImage(jobject bitmap) : mLockedPixel(0), mOwn(false)
    {
        lockJNI();
        gJVM->AttachCurrentThread(&gJNIEnv, NULL);
        mBitmap = gJNIEnv->NewGlobalRef(bitmap);
        unlockJNI();
    }

    AndroidImage::AndroidImage(JNIEnv* env, const char* fileName) : mBitmap(NULL), mLockedPixel(0), mOwn(true)
    {
        lockJNI();
        if (env)
        {
            gJVM->AttachCurrentThread(&env, NULL);
            jclass cls = env->FindClass("com/dwtech/android/houyi/HEngine");
            jmethodID createBitmapMid = env->GetStaticMethodID(cls, "createBitmap", "(Landroid/content/Context;Ljava/lang/String;)Landroid/graphics/Bitmap;");
            jstring jstrBuf = env->NewStringUTF(fileName);
            mBitmap = env->NewGlobalRef(env->CallStaticObjectMethod(cls, createBitmapMid, gContext, jstrBuf));
            if (!mBitmap)
            {
                LOGE("Cannot create Android bitmap. fileName = %s", fileName);
            }
            env->DeleteLocalRef(jstrBuf);
        }
        else
        {
            LOGE("AndroidImage JNIEnv is null");
        }
        unlockJNI();
    }

    AndroidImage::AndroidImage(JNIEnv* env, const int width, const int height) : mBitmap(NULL), mLockedPixel(0), mOwn(true)
    {
        lockJNI();
        if (env)
        {
            gJVM->AttachCurrentThread(&env, NULL);
            jclass cls = env->FindClass("com/dwtech/android/houyi/HEngine");
            jmethodID createBitmapMid = env->GetStaticMethodID(cls, "createBitmap", "(II)Landroid/graphics/Bitmap;");
            mBitmap = env->NewGlobalRef(env->CallStaticObjectMethod(cls, createBitmapMid, width, height));
            if (!mBitmap)
            {
                LOGE("Cannot create Android bitmap. width = %d, height = %d", width, height);
            }
        }
        else
        {
            LOGE("AndroidImage JNIEnv is null");
        }
        unlockJNI();
    }

    AndroidImage::~AndroidImage()
    {
        lockJNI();
        gJVM->AttachCurrentThread(&gJNIEnv, NULL);

        if (mOwn && mBitmap)
        {
            jclass cls = gJNIEnv->FindClass("android/graphics/Bitmap");
            jmethodID mid = gJNIEnv->GetMethodID(cls, "recycle", "()V");
            gJNIEnv->CallVoidMethod(mBitmap, mid);
        }
        gJNIEnv->DeleteGlobalRef(mBitmap);
        unlockJNI();
    }

    int AndroidImage::getWidth() const
    {
        lockJNI();
        JNIEnv* env = gJNIEnv;
        int res = 0;
        if (!env)
        {
            LOGE("AndroidImage::getWidth() JNI env is NULL, call ignite first");
        }
        else if (!mBitmap)
        {
            LOGE("AndroidImage::getWidth() mBitmap is NULL");
        }
        else
        {
            gJVM->AttachCurrentThread(&env, NULL);
            AndroidBitmapInfo info;
            int ret;
            if ((ret = AndroidBitmap_getInfo(env, mBitmap, &info)) < 0)
            {
                LOGE("getWidth AndroidBitmap_getInfo() failed ! error=%d", ret);
            }
            else
            {
                res = info.width;
            }
        }
        unlockJNI();
        return res;
    }

    int AndroidImage::getHeight() const
    {
        lockJNI();
        JNIEnv* env = gJNIEnv;
        int res = 0;
        if (!env)
        {
            LOGE("AndroidImage::getHeight() JNI env is NULL, call ignite first");
        }
        else if (!mBitmap)
        {
            LOGE("AndroidImage::getHeight() mBitmap is NULL");
        }
        else
        {
            gJVM->AttachCurrentThread(&env, NULL);
            AndroidBitmapInfo info;
            int ret;
            if ((ret = AndroidBitmap_getInfo(env, mBitmap, &info)) < 0)
            {
                LOGE("getHeight AndroidBitmap_getInfo() failed ! error=%d", ret);
            }
            else
            {
                res = info.height;
            }
        }
        unlockJNI();
        return res;
    }

    void AndroidImage::lock()
    {
        JNIEnv* env = gJNIEnv;
        if (!env)
        {
            LOGE("AndroidImage::lock() JNI env is NULL, call ignite first");
            return;
        }

        if (mBitmap == NULL)
        {
            LOGE("AndroidImage::lock() mBitmap is null");
            return;
        }

        gJVM->AttachCurrentThread(&env, NULL);
        AndroidBitmapInfo info;
        int ret;
        if ((ret = AndroidBitmap_getInfo(env, mBitmap, &info)) < 0)
        {
            LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
            return;
        }

        if (mLockedPixel)
        {
            LOGE("AndroidBitmap_lockPixels. pixel locked");
            return;
        }

        ret = AndroidBitmap_lockPixels(env, mBitmap, &mLockedPixel);

        if (ret < 0 || !mLockedPixel)
        {
            LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        }
    }

    void AndroidImage::unlock()
    {
        JNIEnv* env = gJNIEnv;
        if (!env)
        {
            LOGE("AndroidImage::unlock() JNI env is NULL, call ignite first");
            return;
        }
        gJVM->AttachCurrentThread(&env, NULL);
        AndroidBitmap_unlockPixels(env, mBitmap);
        mLockedPixel = 0;
    }

    void* AndroidImage::getData() const
    {
        return mLockedPixel;
    }
}
