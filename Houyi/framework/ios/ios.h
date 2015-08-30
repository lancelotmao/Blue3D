//
//  ios.h
//  Houyi
//
//  Created by Lance Mao on 14/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_ios_h
#define Houyi_ios_h

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import <QuartzCore/CAAnimation.h>
#import <UIKit/UIKit.h>

#include <pthread.h>
#include "HouyiiOSImage.h"
#include "HouyiiOSCanvas.h"
#include "HouyiiOSAudioTrack.h"

static int currentPlatform()
{
	return PLATFORM_IOS;
}

static double currentTimeMillis()
{
    return CACurrentMediaTime() * 1000;
}

int getFrameBufferHandle()
{
	int handle = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &handle);
	return handle;
}

int getRenderBufferWidth()
{
	int width = 0;
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	return width;
}

int getRenderBufferHeight()
{
	int height = 0;
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
	return height;
}

float getDensity()
{
    int s = 1;
    UIScreen* screen = [ UIScreen mainScreen ];
    if ( [ screen respondsToSelector:@selector(scale) ] )
        s = (int) [ screen scale ];
    return s;
}
#endif
