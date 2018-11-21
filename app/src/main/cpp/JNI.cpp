#include <jni.h>

#include "BlueWorld.h"
#include "hengine.h"

using namespace Houyi;

extern "C" {
JNIEXPORT jlong JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeCreateWorld(JNIEnv * env, jobject obj);
JNIEXPORT jlong JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeGetFilmstrip(JNIEnv * env, jobject obj, jlong world);
JNIEXPORT void JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeSetViewMode(JNIEnv * env, jobject obj, jlong world, jint viewMode);
JNIEXPORT void JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeInvalidateFilmstrip(JNIEnv * env, jobject obj, jlong world);

JNIEXPORT jlong JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeCreateWorld(JNIEnv * env, jobject obj)
{
	BlueWorld* world = HouyiNew BlueWorld();
	world->create(mRoot);
	return (jlong)world;
}

JNIEXPORT void JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeSetViewMode(JNIEnv * env, jobject obj, jlong world, jint viewMode)
{
    BlueWorld* pWorld = (BlueWorld*)world;
    if (pWorld)
    {
        pWorld->setViewMode(viewMode);
    }
}

JNIEXPORT jlong JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeGetFilmstrip(JNIEnv * env, jobject obj, jlong world)
{
    BlueWorld* pWorld = (BlueWorld*)world;
    if (pWorld)
    {
        return (jlong)pWorld->getFilmstrip();
    }
    return 0;
}

JNIEXPORT void JNICALL Java_com_dwtech_android_blue3d_Blue3DWorld_nativeInvalidateFilmstrip(JNIEnv * env, jobject obj, jlong world)
{
    BlueWorld* pWorld = (BlueWorld*)world;
    if (pWorld && pWorld->getAdapter())
    {
        pWorld->getAdapter()->notifyDataChange();
    }
}
}
