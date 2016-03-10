#include "hengine.h"

using namespace Houyi;

extern "C" {
	JNI_METHOD(HEngine, jlong, ignite)(JNIEnv * env, jobject obj, jobject assetManager, jobject displayMetrics);
	JNI_METHOD(HEngine, void, stallNative)(JNIEnv * env, jobject obj);
	JNI_METHOD(HEngine, void, clearScene)(JNIEnv * env, jobject obj);
	JNI_METHOD(HEngine, void, setContext)(JNIEnv * env, jobject obj, jobject context);
	JNI_METHOD(HEngine, bool, isOn)(JNIEnv * env, jobject obj);
	JNI_METHOD(HEngine, void, onResume)(JNIEnv * env, jobject obj);
	JNI_METHOD(HEngine, void, onPause)(JNIEnv * env, jobject obj);
	JNI_METHOD(HEngine, void, clearContext)(JNIEnv * env, jobject obj);

	JNI_METHOD(HEngine, int, getRootVersion)(JNIEnv * env, jobject obj, jlong pRoot);
    JNI_METHOD(HEngine, jlong, loadProgram)(JNIEnv * env, jobject obj, jint id);
    JNI_METHOD(HEngine, jlong, loadProgram2)(JNIEnv * env, jobject obj, jstring vertexSource, jstring fragmentSource);
    JNI_METHOD(HEngine, void, addPass)(JNIEnv * env, jobject obj, jlong pass);
    JNI_METHOD(HEngine, void, setPass)(JNIEnv * env, jobject obj, jlong pass);
    JNI_METHOD(HEngine, void, deletePtr)(JNIEnv * env, jobject obj, jlong ptr);
    JNI_METHOD(HEngine, void, enableMSAA)(JNIEnv * env, jobject obj, jfloat sampleRate);

    // Root
    JNI_METHOD(HEngine, void, onWindowCreated)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, void, onWindowChanged)(JNIEnv * env, jobject obj, jfloat width, jfloat height);
    JNI_METHOD(HEngine, bool, renderAll)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, bool, beginRender)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, bool, render)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, bool, endRender)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, bool, renderMeshes)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, jlong, getWorldId)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, void, setWorld)(JNIEnv * env, jobject obj, jlong world);
    JNI_METHOD(HEngine, jint, getMeshCount)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, jlong, getMesh)(JNIEnv * env, jobject obj, jint index);
    JNI_METHOD(HEngine, jint, getFPS)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, void, printFPS)(JNIEnv * env, jobject obj, jboolean print);
    JNI_METHOD(HEngine, jint, getPointCount)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, jint, getLineCount)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, jint, getVertexCount)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, jint, getTriangleCount)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, bool, onTouch)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jint t);
    JNI_METHOD(HEngine, bool, onTouch2)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat x2, jfloat y2, jint t);
    JNI_METHOD(HEngine, bool, onDoubleTap)(JNIEnv * env, jobject obj, jfloat x, jfloat y);
    JNI_METHOD(HEngine, bool, onBackPressed)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, bool, isRenderRequested)(JNIEnv * env, jobject obj);
    JNI_METHOD(HEngine, void, requestRender)(JNIEnv * env, jobject obj, bool flag);

    // Renderer
    JNI_METHOD(HouyiRenderer, void, setClearColor)(JNIEnv * env, jobject obj, jfloat r, jfloat g, jfloat b, jfloat a);
    JNI_METHOD(HouyiRenderer, void, disableClearColor)(JNIEnv * env, jobject obj);

    // World
    JNI_METHOD(HouyiWorld, jlong, nativeCreateWorld)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, jlong, getCurrentCamera)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, void, nativeSetCurrentCamera)(JNIEnv * env, jobject obj, jlong camera);
    JNI_METHOD(HouyiWorld, void, addSceneNode)(JNIEnv * env, jobject obj, jlong node);
    JNI_METHOD(HouyiWorld, void, update)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, void, setCubeMapTexture)(JNIEnv * env, jobject obj, jlong cubeMapTexture);
    JNI_METHOD(HouyiWorld, jlong, selectMesh)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat w, jfloat h);
    JNI_METHOD(HouyiWorld, jlong, nativeAddAxis)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, jlong, nativeAddCameraPlane)(JNIEnv * env, jobject obj, jint tid);
    JNI_METHOD(HouyiWorld, void, nativePrepare)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, void, nativeShowAABB)(JNIEnv * env, jobject obj, jboolean show);
    JNI_METHOD(HouyiWorld, void, nativeAddScene)(JNIEnv * env, jobject obj, jobject scene);
    JNI_METHOD(HouyiWorld, jlong, nativeGetFocusScene)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, void, nativeSetFocusScene)(JNIEnv * env, jobject obj, jobject scene);
    JNI_METHOD(HouyiWorld, void, nativeClearScene)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, void, nativeRequestLayout)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiWorld, jlong, nativeGetRootView)(JNIEnv * env, jobject obj);

    // GameWorldOrtho
    JNI_METHOD(HouyiGameWorldOrtho, jlong, nativeCreateWorld)(JNIEnv * env, jobject obj);

    // Object
    JNI_METHOD(HouyiObject, jstring, nativeGetName)(JNIEnv * env, jobject obj);

    // Camera
    JNI_METHOD(HouyiCamera, void, startPeek)(JNIEnv * env, jobject obj, jlong camera);
    JNI_METHOD(HouyiCamera, void, peek)(JNIEnv * env, jobject obj, jlong camera, jfloat x1, jfloat y1, jfloat x2, jfloat y2);
    JNI_METHOD(HouyiCamera, void, endPeek)(JNIEnv * env, jobject obj, jlong camera);
    JNI_METHOD(HouyiCamera, void, setFOV)(JNIEnv * env, jobject obj, jlong camera, jfloat fov);
    JNI_METHOD(HouyiCamera, void, setLookAt)(JNIEnv * env, jobject obj, jlong camera, jfloat x, jfloat y, jfloat z);
    JNI_METHOD(HouyiCamera, void, setPosition)(JNIEnv * env, jobject obj, jlong camera, jfloat x, jfloat y, jfloat z);
    JNI_METHOD(HouyiCamera, void, setUp)(JNIEnv * env, jobject obj, jlong camera, jfloat x, jfloat y, jfloat z);
    JNI_METHOD(HouyiCamera, void, setNearFar)(JNIEnv * env, jobject obj, jlong camera, jfloat near, jfloat far);
    JNI_METHOD(HouyiCamera, void, startPeekAnimation)(JNIEnv * env, jobject obj, jlong camera, jfloat x1, jfloat y1, jfloat x2, jfloat y2, jlong lastTime);
    JNI_METHOD(HouyiCamera, void, stopPeekAnimation)(JNIEnv * env, jobject obj, jlong camera);
    JNI_METHOD(HouyiCamera, void, setPeekConstraint)(JNIEnv * env, jobject obj, jlong camera, jint constraint);
    JNI_METHOD(HouyiCamera, void, nativeStartPresentation)(JNIEnv * env, jobject obj, jfloat speed);
    JNI_METHOD(HouyiCamera, void, nativeStopPresentation)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiCamera, jboolean, nativeIsPresenting)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiOrthographicCamera, jlong, nativeCreateOrthographicCamera)(JNIEnv * env, jobject obj, jfloat l, jfloat r, jfloat t, jfloat b);
    JNI_METHOD(HouyiOrthographicCamera, void, setViewPort)(JNIEnv * env, jobject obj, jfloat l, jfloat r, jfloat t, jfloat b);

    // Geometry
    JNI_METHOD(HouyiGeometry, jlong, nativeCreatePlane)(JNIEnv * env, jobject obj, jint horizontalCount, jint verticalCount, jfloat horizontalUnit, jfloat verticalUnit);
    JNI_METHOD(HouyiGeometry, jlong, createPrime)(JNIEnv * env, jobject obj, jint count);

    // SceneNode
    JNI_METHOD(HouyiSceneNode, jlong, createNode)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiSceneNode, void, nativeSetMesh)(JNIEnv * env, jobject obj, jlong mesh);
    JNI_METHOD(HouyiSceneNode, void, setIdentity)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiSceneNode, void, translate)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z);
    JNI_METHOD(HouyiSceneNode, void, rotate)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z, jfloat r);
    JNI_METHOD(HouyiSceneNode, void, scale)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z);
    JNI_METHOD(HouyiSceneNode, void, setVisibility)(JNIEnv * env, jobject obj, jint visibility);
    JNI_METHOD(HouyiSceneNode, void, setColor)(JNIEnv * env, jobject obj, jobject scene, jfloat r, jfloat g, jfloat b, jfloat a);

    // View
    JNI_METHOD(HouyiView, void, nativeFadeOut)(JNIEnv * env, jobject obj, jfloat duration, jfloat delay, jint interType);

    // CameraNode
    JNI_METHOD(HouyiCameraNode, void, nativeSetPreviewSize)(JNIEnv * env, jobject obj, jfloat w, jfloat h);

    // Mesh
    JNIEXPORT void JNICALL Java_com_dwtech_android_houyi_HouyiMesh_setTexture(JNIEnv * env, jobject obj, jlong mesh, jlong texture);
    JNIEXPORT void JNICALL Java_com_dwtech_android_houyi_HouyiMesh_setRenderMode(JNIEnv * env, jobject obj, jlong mesh, jint renderMode);
    JNI_METHOD(HouyiMesh, void, showAABB)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiMesh, void, hideAABB)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiMesh, void, enableLighting)(JNIEnv * env, jobject obj, jboolean enable);

    // Texture
    JNI_METHOD(HouyiTexture, jlong, createTextureFromAsset)(JNIEnv * env, jobject obj, jstring fileName, jboolean upload);
	JNI_METHOD(HouyiTexture, jlong, nativeCreateTextureFromBitmap)(JNIEnv * env, jobject obj, jobject bitmap, jboolean upload);
	JNI_METHOD(HouyiTexture, jlong, createTextureFromFileDescriptor)(JNIEnv * env, jobject obj, jint fileDescriptor, jboolean upload);
	JNI_METHOD(HouyiTexture, jlong, createTextureFromMemory)(JNIEnv * env, jobject obj, jbyteArray array, jint length, jboolean upload);
	JNI_METHOD(HouyiTexture, jlong, createTextureFromTexture)(JNIEnv * env, jobject obj, jlong texture);
	JNI_METHOD(HouyiTexture, jlong, createTextureFromId)(JNIEnv * env, jobject obj, jint tid);
	JNI_METHOD(HouyiTexture, jlong, createCubeMapFromMemory)(JNIEnv * env, jobject obj, jbyteArray array0, jint length0,
	        jbyteArray array1, jint length1,
	        jbyteArray array2, jint length2,
	        jbyteArray array3, jint length3,
	        jbyteArray array4, jint length4,
	        jbyteArray array5, jint length5);
	JNI_METHOD(HouyiTexture, jlong, createCubeMapFromAsset)(JNIEnv * env, jobject obj,
            jstring fn0, jstring fn1, jstring fn2, jstring fn3, jstring fn4, jstring fn5);
	JNI_METHOD(HouyiTexture, void, setWrapMode)(JNIEnv * env, jobject obj, jint s, jint t);
	JNI_METHOD(HouyiTextureManager, jboolean, hasPendingTexture)(JNIEnv * env, jobject obj);

	// Loader
	JNI_METHOD(HouyiLoader, jlong, nativeCreateLoader)(JNIEnv * env, jobject obj, jstring filePath);
	JNI_METHOD(HouyiLoader, jlong, nativeLoad)(JNIEnv * env, jobject obj, jbyteArray array, jint size, jstring filePath, jint options);
	JNI_METHOD(HouyiLoader, void, nativeCancel)(JNIEnv * env, jobject obj);
	JNI_METHOD(HouyiLoader, jobjectArray, nativeGetTexturePaths)(JNIEnv * env, jobject obj, jbyteArray array, jint size, jstring path);
	JNI_METHOD(HouyiLoader, jfloat, nativeGetProgress)(JNIEnv * env, jobject obj);

	// Saver
	JNI_METHOD(HouyiSaver, bool, nativeSave)(JNIEnv * env, jobject obj, jstring filePath, jobject scene);

	// RenderState
	JNI_METHOD(HouyiRenderState, void, enableDepthTest)(JNIEnv * env, jobject obj, jboolean enable);
	JNI_METHOD(HouyiRenderState, void, enableLighting)(JNIEnv * env, jobject obj, jboolean enable);
	JNI_METHOD(HouyiRenderState, void, enableAlphaBlend)(JNIEnv * env, jobject obj, jboolean enable);

	// Pass
	JNI_METHOD(HouyiPass, jlong, loadProgram)(JNIEnv * env, jobject obj, jint shader);
	JNI_METHOD(HouyiPass, jint, getProgram)(JNIEnv * env, jobject obj);
	JNI_METHOD(HouyiPass, void, setState)(JNIEnv * env, jobject obj, jint state);

	// Setting
	JNI_METHOD(HouyiSetting, void, setShadingMode)(JNIEnv * env, jobject obj, jint mode);
	JNI_METHOD(HouyiSetting, void, setLightingEnabled)(JNIEnv * env, jobject obj, jboolean enable);

	// Scene
	JNI_METHOD(HouyiScene, jlong, nativeAddGrid)(JNIEnv * env, jobject obj);
	JNI_METHOD(HouyiScene, jint, nativeGetSkeletonCount)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiScene, jboolean, nativeIsSkeletonPaused)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiScene, void, nativePauseSkeleton)(JNIEnv * env, jobject obj);
    JNI_METHOD(HouyiScene, void, nativeResumeSkeleton)(JNIEnv * env, jobject obj);

	// SceneManager
	JNI_METHOD(HouyiSceneManager, void, nativeAddScene)(JNIEnv * env, jobject obj, jlong world);
	JNI_METHOD(HouyiSceneManager, jlong, nativeFindSceneByName)(JNIEnv * env, jobject obj, jstring name);
	JNI_METHOD(HouyiSceneManager, jint, nativeGetSceneCount)(JNIEnv * env, jobject obj);
	JNI_METHOD(HouyiSceneManager, jlong, nativeGetScene)(JNIEnv * env, jobject obj, jint index);
	JNI_METHOD(HouyiSceneManager, void, nativeDeleteSceneDefer)(JNIEnv * env, jobject obj, jint index);
};

Root* mRoot;

void lockJNI()
{
    pthread_mutex_lock (&mJNIMutex);
}

void unlockJNI()
{
    pthread_mutex_unlock (&mJNIMutex);
}

void * GetHouyiObject(JNIEnv * env, jobject obj)
{
	if (!g_nHouyiIdField)
	{
		jclass clazz = env->FindClass(HOUYI_OBJECT_CLASS_NAME);
		if(clazz)
		{
			g_nHouyiIdField = env->GetFieldID(clazz, HOUYI_OBJECT_ID_NAME, "J");
		}
	}
	if (g_nHouyiIdField)
	{
		return (void *)env->GetLongField(obj, g_nHouyiIdField);
	}
	return NULL;
}

JNI_METHOD(HEngine, jlong, ignite)(JNIEnv * env, jobject obj, jobject assetManager, jobject metrics)
{
    if (!gInited)
    {
        pthread_mutex_init(&mJNIMutex, 0);
        gInited = true;
    }

    gJNIEnv = env;
    env->GetJavaVM(&gJVM);

    gAssetManager = AAssetManager_fromJava(env, assetManager);
    displayMetrics = env->NewGlobalRef(metrics);

	if (!mRoot)
	{
		mRoot = HouyiNew Root();
		mRoot->init();
	}
	return (jlong)mRoot;
}

JNI_METHOD(HEngine, void, stallNative)(JNIEnv * env, jobject obj)
{
	if (mRoot)
	{
//	    env->DeleteGlobalRef(gContext);
		mRoot->mAutoFinalize = true;
		HouyiDelete(mRoot);
		mRoot = 0;
	}

	env->DeleteGlobalRef(displayMetrics);
	dump_allocator_list();
}

JNI_METHOD(HEngine, void, clearScene)(JNIEnv * env, jobject obj)
{
    if (mRoot)
    {
        mRoot->clearScene();
    }
    dump_allocator_list();
}

JNI_METHOD(HEngine, void, setContext)(JNIEnv * env, jobject obj, jobject context)
{
    gContext = (jobject)env->NewGlobalRef(context);
}

JNI_METHOD(HEngine, bool, isOn)(JNIEnv * env, jobject obj)
{
	return mRoot;
}

JNI_METHOD(HEngine, void, onResume)(JNIEnv * env, jobject obj)
{
    mRoot->onResume();
}

JNI_METHOD(HEngine, void, onPause)(JNIEnv * env, jobject obj)
{
    mRoot->onPause();
}

JNI_METHOD(HEngine, void, clearContext)(JNIEnv * env, jobject obj)
{
    if (mRoot)
    {
        mRoot->clearContext();
    }
    else
    {
        LOGE("clearContext. root is NULL\n");
    }
}

JNI_METHOD(HEngine, int, getRootVersion)(JNIEnv * env, jobject obj, jlong pRoot)
{
	if (!mRoot)
	{
		LOGE("getRootVersion failed, root null");
		return -1;
	}
	return mRoot->mVersion;
}

JNI_METHOD(HEngine, jlong, loadProgram)(JNIEnv * env, jobject obj, jint id)
{
	if (!mRoot)
	{
		LOGE("Root is null. Could not load shader. Call ignite first\n");
		return 0;
	}

	Renderer* renderer = mRoot->getRenderer();
	if (renderer)
	{
		return (jlong)(renderer->loadShader(id));
	}
	else
	{
		LOGE("Could not load shader. No active renderer\n");
		return 0;
	}
}

JNI_METHOD(HEngine, jlong, loadProgram2)(JNIEnv * env, jobject obj, jstring vertexSource, jstring fragmentSource)
{
	if (!mRoot)
	{
		LOGE("Root is null. Could not load shader. Call ignite first\n");
		return 0;
	}

	Renderer* renderer = mRoot->getRenderer();
	if (renderer)
	{
		//USE_JSTRING strFragSrc = USE_JSTRING(env, fragmentSource);
		//LOGE("frag: %s", (const char *)strFragSrc);
		return (jlong)(renderer->loadShader(USE_JSTRING(env, vertexSource), USE_JSTRING(env, fragmentSource)));
	}
	else
	{
		LOGE("Could not load shader. No active renderer\n");
		return 0;
	}
}

JNI_METHOD(HEngine, void, addPass)(JNIEnv * env, jobject obj, jlong pass)
{
    if (!mRoot)
    {
        LOGE("Root is null. Could not addPass. Call ignite first\n");
        return;
    }

    Pass* pPass = (Pass*)pass;
    mRoot->getRenderer()->getShaderManager()->addPass(pPass);
}

JNI_METHOD(HEngine, void, setPass)(JNIEnv * env, jobject obj, jlong pass)
{
	if (!mRoot)
	{
		LOGE("Root is null. Could not setPass. Call ignite first\n");
		return;
	}

	Pass* pPass = (Pass*)pass;
	mRoot->getRenderer()->getShaderManager()->setPass(pPass);
}

JNI_METHOD(HEngine, void, deletePtr)(JNIEnv * env, jobject obj, jlong ptr)
{
	Allocator* p = (Allocator*)ptr;
	HouyiDelete(p);
}

JNI_METHOD(HEngine, void, enableMSAA)(JNIEnv * env, jobject obj, jfloat sampleRate)
{
	if (!mRoot)
	{
		LOGE("enableMSAA. Call ignite first\n");
		return;
	}

	Renderer* renderer = mRoot->getRenderer();
	if (renderer)
	{
		renderer->enableMSAA(sampleRate);
	}
	else
	{
		LOGW("enableMSAA. No active renderer\n");
	}
}

JNI_METHOD(HEngine, void, onWindowCreated)(JNIEnv * env, jobject obj)
{
	if (mRoot)
	{
		mRoot->onWindowCreated();
	}
	else
	{
	    LOGE("onWindowCreated. Call ignite first\n");
	}
}

JNI_METHOD(HEngine, void, onWindowChanged)(JNIEnv * env, jobject obj, jfloat width, jfloat height)
{
	if (mRoot)
	{
		mRoot->onWindowChanged(width, height);
	}
	else
	{
		LOGE("onWindowChanged. Call ignite first\n");
	}
}

JNI_METHOD(HEngine, bool, renderAll)(JNIEnv * env, jobject obj)
{
	if (mRoot)
	{
		mRoot->render();
	}
	else
	{
		LOGE("renderAll. Call ignite first\n");
	}
	return true;
}

JNI_METHOD(HEngine, bool, beginRender)(JNIEnv * env, jobject obj)
{
	if (mRoot && mRoot->getRenderer())
	{
		return mRoot->getRenderer()->beginRender(mRoot->getWorld());
	}
	else
	{
		LOGE("error in beginRender\n");
	}
	return false;
}

JNI_METHOD(HEngine, bool, render)(JNIEnv * env, jobject obj)
{
    if (mRoot && mRoot->getRenderer())
    {
        return mRoot->getRenderer()->render(mRoot->getWorld());
    }
    else
    {
        LOGE("error in render\n");
    }
    return false;
}

JNI_METHOD(HEngine, bool, endRender)(JNIEnv * env, jobject obj)
{
    if (mRoot && mRoot->getRenderer())
    {
        return mRoot->getRenderer()->endRender(mRoot->getWorld());
    }
    else
    {
        LOGE("error in endRender\n");
    }
    return false;
}

JNI_METHOD(HEngine, bool, renderMeshes)(JNIEnv * env, jobject obj)
{
	if (mRoot && mRoot->getRenderer() && mRoot->getWorld())
	{
        return mRoot->getRenderer()->renderScene();
	}
	else
	{
		LOGE("error in renderMeshes\n");
	}
	return false;
}

JNI_METHOD(HEngine, jlong, getWorldId)(JNIEnv * env, jobject obj)
{
	if (mRoot)
	{
		return (jlong)mRoot->getWorld();
	}
	else
	{
		LOGE("Root is null. Could not load shader. Call ignite first\n");
	}
	return 0;
}

JNI_METHOD(HEngine, void, setWorld)(JNIEnv * env, jobject obj, jlong world)
{
	World* worldPtr = (World*)world;
	if (worldPtr)
	{
		mRoot->setWorld(worldPtr);
	}
	else
	{
		LOGE("cannot set NULL world\n");
	}
}

JNI_METHOD(HEngine, jint, getMeshCount)(JNIEnv * env, jobject obj)
{
    World* world = mRoot->getWorld();
	return world->getFocusScene()->getMeshCount();
}

JNI_METHOD(HEngine, jlong, getMesh)(JNIEnv * env, jobject obj, jint index)
{
    World* world = mRoot->getWorld();
	return (jlong)(world->getFocusScene()->getMesh(index));
}

JNI_METHOD(HEngine, jint, getFPS)(JNIEnv * env, jobject obj)
{
	if (mRoot)
	{
		return mRoot->getFPS();
	}
	return 0;
}

JNI_METHOD(HEngine, void, printFPS)(JNIEnv * env, jobject obj, jboolean print)
{
    if (mRoot && mRoot->getRenderer())
    {
        mRoot->printFPS(print);
    }
}

JNI_METHOD(HEngine, jint, getVertexCount)(JNIEnv * env, jobject obj)
{
    if (mRoot && mRoot->getRenderer())
    {
        return mRoot->getRenderer()->getVertexCount();
    }
    return 0;
}

JNI_METHOD(HEngine, jint, getPointCount)(JNIEnv * env, jobject obj)
{
    if (mRoot && mRoot->getRenderer())
    {
        return mRoot->getRenderer()->getPointCount();
    }
    return 0;
}

JNI_METHOD(HEngine, jint, getLineCount)(JNIEnv * env, jobject obj)
{
    if (mRoot && mRoot->getRenderer())
    {
        return mRoot->getRenderer()->getLineCount();
    }
    return 0;
}

JNI_METHOD(HEngine, jint, getTriangleCount)(JNIEnv * env, jobject obj)
{
    if (mRoot && mRoot->getRenderer())
    {
        return mRoot->getRenderer()->getTriangleCount();
    }
    return 0;
}

JNI_METHOD(HEngine, bool, onTouch)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jint t)
{
    if (mRoot)
    {
        MotionEvent e(x, y, t);
        return mRoot->onTouch(e);
    }
    else
    {
        LOGE("onTouch. root is NULL\n");
        return false;
    }
}

JNI_METHOD(HEngine, bool, onTouch2)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat x2, jfloat y2, jint t)
{
    if (mRoot)
    {
        MotionEvent e(x, y, t);
        e.setX(x2, 1);
        e.setY(y2, 1);
        e.mTouchCount = 2;
        return mRoot->onTouch(e);
    }
    else
    {
        LOGE("onTouch. root is NULL\n");
        return false;
    }
}

JNI_METHOD(HEngine, bool, onDoubleTap)(JNIEnv * env, jobject obj, jfloat x, jfloat y)
{
    // TODO use platform gesture
    if (mRoot)
    {
        MotionEvent e(x, y, 0);
        return mRoot->onDoubleTap(e, x, y);
    }
    else
    {
        LOGE("onDoubleTap. root is NULL\n");
        return false;
    }
}

JNI_METHOD(HEngine, bool, onBackPressed)(JNIEnv * env, jobject obj)
{
    if (mRoot)
    {
        return mRoot->onBackPressed();
    }
    else
    {
        LOGE("onBackPressed. root is NULL\n");
        return false;
    }
}

JNI_METHOD(HEngine, bool, isRenderRequested)(JNIEnv * env, jobject obj)
{
    if (mRoot)
    {
        return mRoot->isRenderRequested();
    }
    else
    {
        LOGE("isRenderRequested. root is NULL\n");
        return false;
    }
}

JNI_METHOD(HEngine, void, requestRender)(JNIEnv * env, jobject obj, bool flag)
{
    if (mRoot)
    {
        mRoot->requestRender(flag);
    }
    else
    {
        LOGE("isRenderRequested. root is NULL\n");
    }
}

JNI_METHOD(HouyiRenderer, void, setClearColor)(JNIEnv * env, jobject obj, jfloat r, jfloat g, jfloat b, jfloat a)
{
    if (mRoot && mRoot->getRenderer())
    {
        Color c(r, g, b, a);
        Settings::getInstance()->setClearColor(c);
    }
}

JNI_METHOD(HouyiWorld, jlong, nativeCreateWorld)(JNIEnv * env, jobject obj)
{
	World* world = HouyiNew World();
	world->create(mRoot);
	return (jlong)world;
}

JNI_METHOD(HouyiWorld, jlong, getCurrentCamera)(JNIEnv * env, jobject obj)
{
	World* pWorld = (World *)GetHouyiObject(env, obj);
	if (pWorld)
	{
		return (jlong)pWorld->getCurrentCamera();
	}
	else
	{
		LOGE("HouyiWorld_getCurrentCamera world is not valid\n");
		return 0;
	}
}

JNI_METHOD(HouyiWorld, void, nativeSetCurrentCamera)(JNIEnv * env, jobject obj, jlong camera)
{
    World* pWorld = (World *)GetHouyiObject(env, obj);
    if (pWorld)
    {
        pWorld->getFocusScene()->setCurrentCamera((Camera*)camera);
    }
}

JNI_METHOD(HouyiWorld, void, addSceneNode)(JNIEnv * env, jobject obj, jlong node)
{
	World* pWorld = (World *)GetHouyiObject(env, obj);
	SceneNode* pNode = (SceneNode*)node;
    Mesh* pMesh = pNode->getMesh();
    pWorld->getFocusScene()->addMesh(pMesh);
    pWorld->getFocusScene()->addSceneNode(pNode);
    pWorld->getFocusScene()->addRenderable(pNode);
}

JNI_METHOD(HouyiWorld, void, update)(JNIEnv * env, jobject obj)
{
	World* pWorld = (World *)GetHouyiObject(env, obj);
	if (pWorld)
	{
		pWorld->update();
	}
}

JNI_METHOD(HouyiWorld, void, setCubeMapTexture)(JNIEnv * env, jobject obj, jlong cubeMapTexture)
{
    World* pWorld = (World *)GetHouyiObject(env, obj);
    if (pWorld)
    {
        pWorld->getFocusScene()->setCubeMapTexture((GLESCubeMapTexture*)cubeMapTexture);
    }
}

JNI_METHOD(HouyiWorld, jlong, selectMesh)(JNIEnv * env, jobject obj, jfloat x, jfloat y)
{
    World* pWorld = (World *)GetHouyiObject(env, obj);
    if (pWorld)
    {
    	RenderEntity selected = pWorld->selectMesh(x, y);
    	if (selected != RenderEntity::NullEntity)
    	{
    		return (jlong)selected.mNode;
    	}
    }
    return 0;
}

JNI_METHOD(HouyiWorld, jlong, nativeAddAxis)(JNIEnv * env, jobject obj)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        return (jlong)(pWorld->addAxis());
    }
    return 0;
}

JNI_METHOD(HouyiWorld, jlong, nativeAddCameraPlane)(JNIEnv * env, jobject obj, jint tid)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        return (jlong)(pWorld->addCameraPlane(tid));
    }
    return 0;
}

JNI_METHOD(HouyiWorld, void, nativePrepare)(JNIEnv * env, jobject obj)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        pWorld->getFocusScene()->prepare(0);
    }
}

JNI_METHOD(HouyiWorld, void, nativeShowAABB)(JNIEnv * env, jobject obj, jboolean show)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        if (show)
        {
            pWorld->showAABB();
        }
        else
        {
            pWorld->hideAABB();
        }
    }
}

JNI_METHOD(HouyiWorld, void, nativeAddScene)(JNIEnv * env, jobject obj, jobject scene)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    Scene* pScene = (Scene*)GetHouyiObject(env, scene);
    if (pWorld && pScene)
    {
        pWorld->addScene(pScene);
    }
}

JNI_METHOD(HouyiWorld, jlong, nativeGetFocusScene)(JNIEnv * env, jobject obj)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        return (jlong)(pWorld->getFocusScene());
    }
    return 0;
}

JNI_METHOD(HouyiWorld, void, nativeSetFocusScene)(JNIEnv * env, jobject obj, jobject scene)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    Scene* pScene = (Scene*)GetHouyiObject(env, scene);
    if (pWorld && pScene)
    {
        pWorld->setFocusScene(pScene);
    }
}

JNI_METHOD(HouyiWorld, void, nativeClearScene)(JNIEnv * env, jobject obj)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        pWorld->clearScene();
    }
}

JNI_METHOD(HouyiWorld, void, nativeRequestLayout)(JNIEnv * env, jobject obj)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        pWorld->getRootView()->requestLayout();
    }
}

JNI_METHOD(HouyiWorld, jlong, nativeGetRootView)(JNIEnv * env, jobject obj)
{
    World* pWorld = (World*)GetHouyiObject(env, obj);
    if (pWorld)
    {
        return (jlong)pWorld->getRootView();
    }
    return 0;
}

// GameWorldOrtho
JNI_METHOD(HouyiGameWorldOrtho, jlong, nativeCreateWorld)(JNIEnv * env, jobject obj)
{
    GameWorld* world = HouyiNew GameWorld();
    return (jlong)world;
}

JNI_METHOD(HouyiObject, jstring, nativeGetName)(JNIEnv * env, jobject obj)
{
    Object* pObj = (Object*)GetHouyiObject(env, obj);
    if (pObj)
    {
        string name = pObj->getName();
        return env->NewStringUTF(name.c_str());
    }
    return 0;
}

JNI_METHOD(HouyiCamera, void, startPeek)(JNIEnv * env, jobject obj, jlong camera)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->startPeek();
    }
    else
    {
        LOGE("HouyiCamera_startPeek camera is not valid\n");
    }
}

JNI_METHOD(HouyiCamera, void, peek)(JNIEnv * env, jobject obj, jlong camera, jfloat x1, jfloat y1, jfloat x2, jfloat y2)
{
//	LOGD("Peek (%f, %f), (%f, %f)", x1, y1, x2, y2);
	Camera* pCamera = (Camera*)camera;
	if (pCamera)
	{
		pCamera->peek(x1, y1, x2, y2);
	}
	else
	{
		LOGE("HouyiCamera_peek camera is not valid\n");
	}
}

JNI_METHOD(HouyiCamera, void, endPeek)(JNIEnv * env, jobject obj, jlong camera)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->endPeek();
    }
    else
    {
        LOGE("HouyiCamera_endPeek camera is not valid\n");
    }
}

JNI_METHOD(HouyiCamera, void, setFOV)(JNIEnv * env, jobject obj, jlong camera, jfloat fov)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->setFOV(fov);
    }
}

JNI_METHOD(HouyiCamera, void, setLookAt)(JNIEnv * env, jobject obj, jlong camera, jfloat x, jfloat y, jfloat z)
{
	Camera* pCamera = (Camera*)camera;
	if (pCamera)
	{
		pCamera->setLookAt(x, y, z);
	}
	else
	{
		LOGE("HouyiCamera_setLookAt camera is not valid\n");
	}
}

JNI_METHOD(HouyiCamera, void, setPosition)(JNIEnv * env, jobject obj, jlong camera, jfloat x, jfloat y, jfloat z)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->setPosition(x, y, z);
    }
    else
    {
        LOGE("HouyiCamera_setPosition camera is not valid\n");
    }
}


JNI_METHOD(HouyiCamera, void, setUp)(JNIEnv * env, jobject obj, jlong camera, jfloat x, jfloat y, jfloat z)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->setUp(x, y, z);
    }
    else
    {
        LOGE("HouyiCamera_setUp camera is not valid\n");
    }
}

JNI_METHOD(HouyiCamera, void, setNearFar)(JNIEnv * env, jobject obj, jlong camera, jfloat near, jfloat far)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->setNearRange(near);
        pCamera->setFarRange(far);
    }
    else
    {
        LOGE("HouyiCamera_setUp camera is not valid\n");
    }
}

JNI_METHOD(HouyiCamera, void, startPeekAnimation)(JNIEnv * env, jobject obj, jlong camera, jfloat x1, jfloat y1, jfloat x2, jfloat y2, jlong lastTime)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->startPeekAnim(x1, y1, x2, y2, lastTime, currentTimeMillis());
    }
    else
    {
        LOGE("HouyiCamera_startPeekAnimation camera is not valid\n");
    }
}

JNI_METHOD(HouyiCamera, void, stopPeekAnimation)(JNIEnv * env, jobject obj, jlong camera)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->stopPeekAnim();
    }
    else
    {
        LOGE("HouyiCamera_stopPeekAnimation camera is not valid\n");
    }
}

JNI_METHOD(HouyiCamera, void, setPeekConstraint)(JNIEnv * env, jobject obj, jlong camera, jint constraint)
{
    Camera* pCamera = (Camera*)camera;
    if (pCamera)
    {
        pCamera->setPeekConstraint(constraint);
    }
    else
    {
        LOGE("setPeekConstrait camera is not valid\n");
    }
}

JNI_METHOD(HouyiCamera, void, nativeStartPresentation)(JNIEnv * env, jobject obj, jfloat speed)
{
	Camera* cam = (Camera*)GetHouyiObject(env, obj);
	if (cam)
	{
		cam->getAnimController().startPresentation();
	}
}

JNI_METHOD(HouyiCamera, void, nativeStopPresentation)(JNIEnv * env, jobject obj)
{
	Camera* cam = (Camera*)GetHouyiObject(env, obj);
	if (cam)
	{
		cam->getAnimController().stopPresentation();
	}
}

JNI_METHOD(HouyiCamera, jboolean, nativeIsPresenting)(JNIEnv * env, jobject obj)
{
	Camera* cam = (Camera*)GetHouyiObject(env, obj);
	return cam ? cam->getAnimController().isPresenting() : false;
}

JNI_METHOD(HouyiOrthographicCamera, jlong, nativeCreateOrthographicCamera)(JNIEnv * env, jobject obj, jfloat l, jfloat r, jfloat t, jfloat b)
{
    OrthographicCamera* cam = HouyiNew OrthographicCamera(l, r, t, b);
    return (jlong)cam;
}

JNI_METHOD(HouyiOrthographicCamera, void, setViewPort)(JNIEnv * env, jobject obj, jfloat l, jfloat r, jfloat t, jfloat b)
{
    OrthographicCamera* camera = (OrthographicCamera*)GetHouyiObject(env, obj);
    if (camera)
    {
        camera->setViewPort(l, r, t, b);
    }
}

JNI_METHOD(HouyiGeometry, jlong, nativeCreatePlane)(JNIEnv * env, jobject obj, jint horizontalCount, jint verticalCount, jfloat horizontalUnit, jfloat verticalUnit)
{
	GeoPlane* plane = HouyiNew GeoPlane(horizontalCount, verticalCount, horizontalUnit, verticalUnit);
	return (long)plane;
}

JNI_METHOD(HouyiGeometry, jlong, createPrime)(JNIEnv * env, jobject obj, jint count)
{
    Prime* prime = HouyiNew Prime(count);
    prime->generate();
    return (long)prime;
}

// SceneNode
JNI_METHOD(HouyiSceneNode, jlong, createNode)(JNIEnv * env, jobject obj)
{
    return (jlong)HouyiNew SceneNode();
}

JNI_METHOD(HouyiSceneNode, void, nativeSetMesh)(JNIEnv * env, jobject obj, jlong mesh)
{
    SceneNode* pNode = (SceneNode*)GetHouyiObject(env, obj);
    Mesh* pMesh = (Mesh*)mesh;
    pNode->setMesh(pMesh);
}

JNI_METHOD(HouyiSceneNode, void, setIdentity)(JNIEnv * env, jobject obj)
{
    SceneNode* pNode = (SceneNode*)GetHouyiObject(env, obj);
    if (pNode)
    {
        pNode->setIdentity();
    }
}

JNI_METHOD(HouyiSceneNode, void, translate)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z)
{
    SceneNode* pNode = (SceneNode*)GetHouyiObject(env, obj);
    if (pNode)
    {
        pNode->translate(x, y, z);
    }
}

JNI_METHOD(HouyiSceneNode, void, rotate)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z, jfloat r)
{
    SceneNode* pNode = (SceneNode*)GetHouyiObject(env, obj);
    if (pNode)
    {
        pNode->rotate(x, y, z, r);
    }
}

JNI_METHOD(HouyiSceneNode, void, scale)(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z)
{
    SceneNode* pNode = (SceneNode*)GetHouyiObject(env, obj);
    if (pNode)
    {
        pNode->scale(x, y, z);
    }
}

JNI_METHOD(HouyiSceneNode, void, setVisibility)(JNIEnv * env, jobject obj, jint visibility)
{
    SceneNode* pNode = (SceneNode*)GetHouyiObject(env, obj);
    if (pNode)
    {
        pNode->setVisibility((SceneNode::Visibility)visibility);
    }
}

JNI_METHOD(HouyiSceneNode, void, setColor)(JNIEnv * env, jobject obj, jobject scene, jfloat r, jfloat g, jfloat b, jfloat a)
{
    SceneNode* pNode = (SceneNode*)GetHouyiObject(env, obj);
    Scene* pScene = (Scene*)GetHouyiObject(env, scene);
    if (pNode)
    {
        pNode->setColor(pScene, Color(r, g, b, a));
    }
}

// View
JNI_METHOD(HouyiView, void, nativeFadeOut)(JNIEnv * env, jobject obj, jfloat duration, jfloat delay, jint interType)
{
    View* pView = (View*)GetHouyiObject(env, obj);
    if (pView)
    {
        pView->fadeOut(duration, delay, interType);
    }
}

JNI_METHOD(HouyiCameraNode, void, nativeSetPreviewSize)(JNIEnv * env, jobject obj, jfloat w, jfloat h)
{
    CameraNode* pNode = (CameraNode*)GetHouyiObject(env, obj);
    if (pNode)
    {
        pNode->setPreviewSize(w, h);
    }
}

// Mesh
JNIEXPORT void JNICALL Java_com_dwtech_android_houyi_HouyiMesh_setTexture(JNIEnv * env, jobject obj, jlong mesh, jlong texture)
{
	Mesh* pMesh = (Mesh*)mesh;
	Texture* pTexture = (Texture*)texture;
	if (pMesh && pTexture)
	{
		pMesh->setTexture(pTexture);
	}
}

JNIEXPORT void JNICALL Java_com_dwtech_android_houyi_HouyiMesh_setRenderMode(JNIEnv * env, jobject obj, jlong mesh, jint renderMode)
{
	Mesh* pMesh = (Mesh*)mesh;
	if (pMesh)
	{
	    for (int i = 0;i < pMesh->getSurfaceCount();++i)
	    {
	        pMesh->getSurface(i)->setRenderMode(renderMode);
	    }
	}
}

JNI_METHOD(HouyiMesh, void, showAABB)(JNIEnv * env, jobject obj)
{
    Mesh* pMesh = (Mesh*)GetHouyiObject(env, obj);
    if (pMesh)
    {
        // TODO
//        pMesh->showAABB();
    }
}

JNI_METHOD(HouyiMesh, void, hideAABB)(JNIEnv * env, jobject obj)
{
    Mesh* pMesh = (Mesh*)GetHouyiObject(env, obj);
    if (pMesh)
    {
        // TODO
//        pMesh->hideAABB();
    }
}

// Texture
JNI_METHOD(HouyiTexture, jlong, createTextureFromAsset)(JNIEnv * env, jobject obj, jstring fileName, jboolean upload)
{
    const char* strFileName = env->GetStringUTFChars(fileName, NULL);
    ImagePtr bitmap = ImageFactory::createImage(strFileName);
    if (!bitmap)
    {
        LOGE("Cannot create bitmap from file. fileName = %s", strFileName);
        return 0;
    }
    Texture* texture = mRoot->getWorld()->getFocusScene()->getTextureManager()->createTexture(bitmap);
    return (jlong)texture;
}

JNI_METHOD(HouyiTexture, jlong, nativeCreateTextureFromBitmap)(JNIEnv * env, jobject obj, jobject bitmap, jboolean upload)
{
    if (!bitmap)
    {
        LOGE("createTextureFromBitmap NULL bitmap");
        return 0;
    }

    ImagePtr image = ImageFactory::createImage(bitmap);
    Texture* texture = mRoot->getWorld()->getFocusScene()->getTextureManager()->createTexture(image);
    return (jlong)texture;
}

JNI_METHOD(HouyiTexture, jlong, createTextureFromMemory)(JNIEnv * env, jobject obj, jbyteArray array, jint length, jboolean upload)
{
	jlong texturePtr = 0;
	ImagePtr ip = ImageFactory::createImage("test.jpg");
	if (!ip)
	{
	    LOGW("Cannot create bitmap from memory\n");
	}
	else
	{
	    Texture* texture = mRoot->getWorld()->getFocusScene()->getTextureManager()->createTexture(ip);
        texturePtr = (jlong)texture;
	}

	return texturePtr;
}

JNI_METHOD(HouyiTexture, jlong, createTextureFromTexture)(JNIEnv * env, jobject obj, jlong texture)
{
	/*Texture* tPtr = (Texture*)texture;
	if (tPtr)
	{
		Texture* ret = HouyiNew GLESTexture(tPtr->getTextureId());
		ret->setUScaleFactor(tPtr->getUScaleFactor());
		ret->setVScaleFactor(tPtr->getVScaleFactor());
		return (jlong)ret;
	}
	return 0;*/
    return 0;
}

JNI_METHOD(HouyiTexture, jlong, createTextureFromId)(JNIEnv * env, jobject obj, jint tid)
{
    /*Texture* ret = HouyiNew Texture(tid);
    return (jlong)ret;*/
    return 0;
}

JNI_METHOD(HouyiTexture, jlong, createCubeMapFromMemory)(JNIEnv * env, jobject obj, jbyteArray array0, jint length0,
            jbyteArray array1, jint length1,
            jbyteArray array2, jint length2,
            jbyteArray array3, jint length3,
            jbyteArray array4, jint length4,
            jbyteArray array5, jint length5)
{
    /*jbyteArray array[6] = {array0, array1, array2, array3, array4, array5};
    jint length[6] = {length0, length1, length2, length3, length4, length5};
    SkBitmap* images[6];
    for (int i = 0;i < 6;++i)
    {
        jbyte* data = env->GetByteArrayElements(array[i], NULL);
        images[i] = new SkBitmap();
        bool ret = SkImageDecoder::DecodeMemory(data, length[i], images[i]);
        if (!ret)
        {
            delete images[i];
            images[i] = 0;
            LOGW("createCubeMapFromMemory Cannot create bitmap from memory i = %d\n", i);
        }
        env->ReleaseByteArrayElements(array[i], data, JNI_ABORT);
    }

    GLESCubeMapTexture* cmt = HouyiNew GLESCubeMapTexture(images);
    return (jlong)cmt;*/
    return 0;
}

JNI_METHOD(HouyiTexture, jlong, createCubeMapFromAsset)(JNIEnv * env, jobject obj,
            jstring fn0, jstring fn1, jstring fn2, jstring fn3, jstring fn4, jstring fn5)
{
    jstring names[6] = {fn0, fn1, fn2, fn3, fn4, fn5};
    ImagePtr images[6];
    for (int i = 0;i < 6;++i)
    {
        const char* strFileName = env->GetStringUTFChars(names[i], NULL);
        images[i] = ImageFactory::createImage(strFileName);
    }

    GLESCubeMapTexture* cmt = HouyiNew GLESCubeMapTexture(images);
    return (jlong)cmt;
}

JNI_METHOD(HouyiTexture, void, setWrapMode)(JNIEnv * env, jobject obj, jint s, jint t)
{
    Texture* pTexture = (Texture*)GetHouyiObject(env, obj);
    pTexture->setWrapMode(s, t);
}

JNI_METHOD(HouyiTextureManager, jboolean, hasPendingTexture)(JNIEnv * env, jobject obj)
{
    TextureManager* tm = mRoot->getWorld()->getFocusScene()->getTextureManager();
    return tm->hasPendingTexture();
}

// Loader
JNI_METHOD(HouyiLoader, jlong, nativeCreateLoader)(JNIEnv * env, jobject obj, jstring filePath)
{
	const char* strFileName = env->GetStringUTFChars(filePath, NULL);
	Loader* loader = Loader::getLoader(string(strFileName));
	return (jlong)loader;
}

JNI_METHOD(HouyiLoader, jlong, nativeLoad)(JNIEnv * env, jobject obj, jbyteArray array, jint size, jstring filePath, jint options)
{
	Loader* pLoader = (Loader*)GetHouyiObject(env, obj);
	if (!pLoader)
	{
		LOGE("HouyiLoader::load. invalid loader\n");
		return 0;
	}

	jbyte* data = env->GetByteArrayElements(array, NULL);
	Scene* ret = pLoader->load((const char*)data, size, options);
	if (!ret)
	{
		LOGW("Cannot load scene\n");
	}
	env->ReleaseByteArrayElements(array, data, JNI_ABORT);
	return (jlong)ret;
}

JNI_METHOD(HouyiLoader, void, nativeCancel)(JNIEnv * env, jobject obj)
{
	Loader* pLoader = (Loader*)GetHouyiObject(env, obj);
	if (pLoader)
	{
		pLoader->cancel();
	}
}

JNI_METHOD(HouyiLoader, jobjectArray, nativeGetTexturePaths)(JNIEnv * env, jobject obj, jbyteArray array, jint size, jstring path)
{
	jbyte* data = env->GetByteArrayElements(array, NULL);
	TextureImageChecker checker;
	Scene* world = checker.loadFromMemory((const char*)data, size);
	env->ReleaseByteArrayElements(array, data, JNI_ABORT);
	jobjectArray ret;
	if (world)
	{
		int count = checker.mCollada.mImages.size();
		ret = (jobjectArray)env->NewObjectArray(count * 2,
				env->FindClass("java/lang/String"), env->NewStringUTF(""));

		if (count > 0)
		{
			string strPath = string(env->GetStringUTFChars(path, NULL));
			for(int i=0;i<count;i++)
			{
				collada::Image img = checker.mCollada.mImages[i];
				const string str = img.mInitFrom;
				string textureFullPath = StringUtils::getTexturePath(strPath, str);
				LOGI("found texture full path = %s", textureFullPath.c_str());
				env->SetObjectArrayElement(ret,i*2,env->NewStringUTF(textureFullPath.c_str()));

				string textureName = StringUtils::getLastPathComponent(str);
				string filePath = StringUtils::deleteLastPathComponent(strPath);
				string textureCurPath = filePath +textureName;
				LOGI("found texture cur path = %s", textureCurPath.c_str());
				env->SetObjectArrayElement(ret,i*2+1,env->NewStringUTF(textureCurPath.c_str()));
			}
		}

		HouyiDelete(world);
	}
	else
	{
		LOGE("HouyiLoader::getImagePathsInvalid collada file\n");
	}
	return(ret);
}

JNI_METHOD(HouyiLoader, jfloat, nativeGetProgress)(JNIEnv * env, jobject obj)
{
    Loader* pLoader = (Loader*)GetHouyiObject(env, obj);
    if (pLoader)
    {
        return pLoader->getProgress();
    }
    return 0;
}

// Saver
JNI_METHOD(HouyiSaver, bool, nativeSave)(JNIEnv * env, jobject obj, jstring filePath, jobject scene)
{
    Scene* pScene = (Scene*)GetHouyiObject(env, scene);
    const char* strFileName = env->GetStringUTFChars(filePath, NULL);
    return Saver::getSaver(strFileName)->save(pScene);
}

// Pass
JNI_METHOD(HouyiPass, jlong, loadProgram)(JNIEnv * env, jobject obj, jint shader)
{
    if (!mRoot)
    {
        LOGE("Root is null. Could not load shader. Call ignite first\n");
        return 0;
    }

    Renderer* renderer = mRoot->getRenderer();
    if (renderer)
    {
        return (jlong)(renderer->loadShader(shader));
    }
    else
    {
        LOGE("Could not load shader. No active renderer\n");
        return 0;
    }
}

JNI_METHOD(HouyiPass, jint, getProgram)(JNIEnv * env, jobject obj)
{
    Pass* pPass = (Pass*)GetHouyiObject(env, obj);
    if (pPass)
    {
        return pPass->getProgram();
    }
    else
    {
        LOGE("getProgram. Pass is not valid");
    }
    return -1;
}

JNI_METHOD(HouyiPass, void, setState)(JNIEnv * env, jobject obj, jint state)
{
    Pass* pPass = (Pass*)GetHouyiObject(env, obj);
    if (pPass)
    {
        return pPass->setPassState(state);
    }
    else
    {
        LOGE("getProgram. Pass is not valid");
    }
}

// Setting
JNI_METHOD(HouyiSetting, void, setShadingMode)(JNIEnv * env, jobject obj, jint mode)
{
    Settings::getInstance()->setShadingMode(mode);
}

JNI_METHOD(HouyiSetting, void, setLightingEnabled)(JNIEnv * env, jobject obj, jboolean enable)
{
    Settings::getInstance()->setLightingEnabled(enable);
}

// Scene
JNI_METHOD(HouyiScene, jlong, nativeAddGrid)(JNIEnv * env, jobject obj)
{
    Scene* pScene = (Scene*)GetHouyiObject(env, obj);
    if (pScene)
    {
        return (jlong)(pScene->addGrid());
    }
    return 0;
}

JNI_METHOD(HouyiScene, jint, nativeGetSkeletonCount)(JNIEnv * env, jobject obj)
{
    Scene* pScene = (Scene*)GetHouyiObject(env, obj);
    if (pScene)
    {
        return pScene->getSkeletonCount();
    }
    return 0;
}

JNI_METHOD(HouyiScene, jboolean, nativeIsSkeletonPaused)(JNIEnv * env, jobject obj)
{
    Scene* pScene = (Scene*)GetHouyiObject(env, obj);
    if (pScene)
    {
        return pScene->isSkeletonPaused();
    }
    return true;
}

JNI_METHOD(HouyiScene, void, nativePauseSkeleton)(JNIEnv * env, jobject obj)
{
    Scene* pScene = (Scene*)GetHouyiObject(env, obj);
    if (pScene)
    {
        return pScene->pauseSkeleton();
    }
}

JNI_METHOD(HouyiScene, void, nativeResumeSkeleton)(JNIEnv * env, jobject obj)
{
    Scene* pScene = (Scene*)GetHouyiObject(env, obj);
    if (pScene)
    {
        return pScene->resumeSkeleton();
    }
}

// SceneManager
JNI_METHOD(HouyiSceneManager, void, nativeAddScene)(JNIEnv * env, jobject obj, jlong scene)
{
    Scene* pScene = (Scene*)scene;
	SceneManager::getInstance()->addScene(pScene);
}

JNI_METHOD(HouyiSceneManager, jlong, nativeFindSceneByName)(JNIEnv * env, jobject obj, jstring name)
{
    string str = string(env->GetStringUTFChars(name, NULL));
    Scene* scene = SceneManager::getInstance()->findSceneByName(str);
    return (jlong)scene;
}

JNI_METHOD(HouyiSceneManager, jint, nativeGetSceneCount)(JNIEnv * env, jobject obj)
{
    return SceneManager::getInstance()->getSceneCount();
}

JNI_METHOD(HouyiSceneManager, jlong, nativeGetScene)(JNIEnv * env, jobject obj, jint index)
{
    Scene* scene = SceneManager::getInstance()->getScene(index);
    return (jlong)scene;
}

JNI_METHOD(HouyiSceneManager, void, nativeDeleteSceneDefer)(JNIEnv * env, jobject obj, jint index)
{
    SceneManager::getInstance()->deleteSceneDefer(index);
}
