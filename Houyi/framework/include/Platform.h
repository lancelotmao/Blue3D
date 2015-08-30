#ifndef Houyi_Platform_h
#define Houyi_Platform_h

#define PLATFORM_ANDROID	0
#define PLATFORM_IOS		1

static int currentPlatform();
static double currentTimeMillis();
static int getFrameBufferHandle();
static int getRenderBufferWidth();
static int getRenderBufferHeight();
static float getDensity();

#ifdef ANDROID
#include "android.h"
#elif __APPLE__
#include "ios.h"
#endif

#endif
