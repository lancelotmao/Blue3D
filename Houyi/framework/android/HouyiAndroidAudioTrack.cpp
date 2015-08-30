#include "HouyiLog.h"
#include "HouyiAndroidAudioTrack.h"

namespace Houyi
{
    AndroidAudioTrack::AndroidAudioTrack(const string fileName) : AudioTrack(fileName)
    {
        gJVM->AttachCurrentThread(&gJNIEnv, NULL);
        jclass cls = gJNIEnv->FindClass("com/dwtech/android/houyi/HouyiAudioTrack");
        jmethodID constructor = gJNIEnv->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
        jstring jstrBuf = gJNIEnv->NewStringUTF(mFileName.c_str());
        mJAudioTrack = gJNIEnv->NewGlobalRef(gJNIEnv->NewObject(cls, constructor, jstrBuf));
        gJNIEnv->DeleteLocalRef(jstrBuf);
        prepare();
    }

    void AndroidAudioTrack::prepare()
    {
        static const int BUFSIZE = 1024 * 100;
        AAsset* asset = AAssetManager_open(gAssetManager, mFileName.c_str(), AASSET_MODE_STREAMING);
        char buf[BUFSIZE];
        int total_count = 0;
        int nb_read = 0;
        while ((nb_read = AAsset_read(asset, buf, BUFSIZE)) > 0)
        {
            total_count += nb_read;
        }
        AAsset_close(asset);

        char* data = new char[total_count];
        asset = AAssetManager_open(gAssetManager, mFileName.c_str(), AASSET_MODE_STREAMING);
        nb_read = 0;
        int index = 0;
        while ((nb_read = AAsset_read(asset, data + index, BUFSIZE)) > 0)
        {
            index += nb_read;
        }
        AAsset_close(asset);

        AudioTrack::decode(data, total_count);
    }

    void AndroidAudioTrack::play(int loop)
    {
        mLoop = loop;

        if (gJNIEnv)
		{
			gJVM->AttachCurrentThread(&gJNIEnv, NULL);
			jclass cls = gJNIEnv->FindClass("com/dwtech/android/houyi/HouyiAudioTrack");
			jmethodID mid = gJNIEnv->GetMethodID(cls, "play", "(Landroid/content/Context;)V");
			gJNIEnv->CallVoidMethod(mJAudioTrack, mid, gContext);
		}
		else
		{
			LOGE("AndroidSoundPlayer play JNIEnv is null");
		}
    }

    void AndroidAudioTrack::pause()
    {

    }

    void AndroidAudioTrack::stop()
    {
    	if (gJNIEnv)
		{
			gJVM->AttachCurrentThread(&gJNIEnv, NULL);
			jclass cls = gJNIEnv->FindClass("com/dwtech/android/houyi/HouyiAudioTrack");
			jmethodID mid = gJNIEnv->GetMethodID(cls, "stop", "()V");
			gJNIEnv->CallVoidMethod(mJAudioTrack, mid);
		}
		else
		{
			LOGE("AndroidSoundPlayer stop JNIEnv is null");
		}
    }

    void AndroidAudioTrack::setVolume(float volume)
    {
        mVolume = volume;
    }

    int AndroidAudioTrack::getCurrentPosition()
    {
        if (gJNIEnv)
        {
            gJVM->AttachCurrentThread(&gJNIEnv, NULL);
            jclass cls = gJNIEnv->FindClass("com/dwtech/android/houyi/HouyiAudioTrack");
            jmethodID mid = gJNIEnv->GetMethodID(cls, "getCurrentPosition", "()I");
            return gJNIEnv->CallIntMethod(mJAudioTrack, mid);
        }
        else
        {
            LOGE("AndroidSoundPlayer getCurrentPosition JNIEnv is null");
        }
        return 0;
    }

    float AndroidAudioTrack::getDuration()
    {
        if (gJNIEnv)
        {
            gJVM->AttachCurrentThread(&gJNIEnv, NULL);
            jclass cls = gJNIEnv->FindClass("com/dwtech/android/houyi/HouyiAudioTrack");
            jmethodID mid = gJNIEnv->GetMethodID(cls, "getDuration", "()F");
            return gJNIEnv->CallFloatMethod(mJAudioTrack, mid);
        }
        else
        {
            LOGE("AndroidSoundPlayer getCurrentPosition JNIEnv is null");
        }
        return 0;
    }
}
