#include <stdio.h>
#include <stdlib.h>

#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <time.h>
#include "assert.h"

#include "HouyiAndroidImage.h"
#include "HouyiAndroidCanvas.h"
#include "HouyiAndroidAudioTrack.h"

// defined in hengine.cpp
extern JNIEnv* gJNIEnv;
extern JavaVM* gJVM;
extern jobject gContext;
extern AAssetManager* gAssetManager;
extern jobject displayMetrics;

int currentPlatform()
{
	return PLATFORM_ANDROID;
}

double currentTimeMillis()
{
	struct timespec res;
	clock_gettime(CLOCK_REALTIME, &res);
	return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}

int getFrameBufferHandle()
{
	return 0;
}

int getRenderBufferWidth()
{
	return 0;
}

int getRenderBufferHeight()
{
	return 0;
}

void lockJNI();
void unlockJNI();

float getDensity()
{
	lockJNI();
    gJVM->AttachCurrentThread(&gJNIEnv, NULL);
	jclass displayMetricsClass = gJNIEnv->GetObjectClass(displayMetrics);
	jfieldID densityId = gJNIEnv->GetFieldID(displayMetricsClass, "density", "F");
	float density = gJNIEnv->GetFloatField(displayMetrics, densityId);
	unlockJNI();
	return density;
}
