//
//  HouyiGLUtils.h
//  Houyi
//
//  Created by Lance Mao on 14/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiGLUtils_h
#define Houyi_HouyiGLUtils_h

#include "HouyiLog.h"
#include "HouyiRenderTarget.h"

#ifdef DEBUG
#define CHECK_GL_ERROR(a) checkGlError(a, __FILE__, __LINE__)
#else
#define CHECK_GL_ERROR(a)
#endif

namespace Houyi
{
    inline void printGLString(const char *name, GLenum s)
    {
        const char *v = (const char *) glGetString(s);
        LOGI("GL %s = %s\n", name, v);
    }

    inline void checkGlError(const char* op)
    {
        for (GLint error = glGetError(); error; error = glGetError())
        {
            LogOnce("after %s() glError (0x%x)\n", op, error);
        }
    }

    inline void checkGlError(const char* op, const char* file, int line)
    {
        for (GLint error = glGetError(); error; error = glGetError())
        {
            LogOnce("GL ERROR:%s() glError (0x%x) File=%s, Line=%d\n", op, error, file, line);
        }
    }

    class GLUtils
    {
    public:
        static int loadShader(const char* pVertexSource, const char* pFragmentSource);
        static GLuint loadShader(GLenum shaderType, const char* pSource);
        static RenderTarget* createRenderTarget(int width, int height, bool ensurePOT = false);
    };
}

#endif
