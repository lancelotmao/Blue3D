#include "HouyiGLUtils.h"

namespace Houyi
{
    int GLUtils::loadShader(const char* pVertexSource, const char* pFragmentSource)
    {
        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
        if (vertexShader)
        {
            LOGI ("Creating program. vertexShader created\n");
        }
        else
        {
            LOGE ("create vertex shader failed\n");
            return 0;
        }

        GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
        if (pixelShader)
        {
            LOGI ("Creating program. fragmentShader created\n");
        }
        else
        {
            LOGE ("create fragment shader failed\n");
            return 0;
        }

        GLuint program = glCreateProgram();
        if (program)
        {
            glAttachShader(program, vertexShader);
            checkGlError("glAttachShader");
            glAttachShader(program, pixelShader);
            checkGlError("glAttachShader");
            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc(bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGE("Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
        LOGI ("Program created successfully\n");
        return program;
    }

    GLuint GLUtils::loadShader(GLenum shaderType, const char* pSource)
    {
        GLuint shader = glCreateShader(shaderType);
        if (shader)
        {
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
            {
                GLint infoLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen)
                {
                    char* buf = (char*) malloc(infoLen);
                    if (buf)
                    {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf);
                        LOGE("Could not compile shader %d:\n%s\n",
                                shaderType, buf);
                        free(buf);
                    }
                    glDeleteShader(shader);
                    shader = 0;
                }
            }
        }
        return shader;
    }

    RenderTarget* GLUtils::createRenderTarget(int width, int height, bool ensurePOT)
    {
        checkGlError("enter createRenderTarget");
        // set up our frame buffer
        // General frame buffer
        GLuint handle[1];
        glGenFramebuffers(1, handle);
        checkGlError("generate framebuffer");

        // Bind frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, handle[0]);
        checkGlError("bind framebuffer");

        // add a texture to render to
        GLuint textures[2];
        glGenTextures(2, textures);

        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//        if (mIsTrilinearFilterEnabled)
//        {
//            LOGD("createRenderTarget enabling trilinear filtering\n");
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        }
//        else
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        checkGlError("create RT texture");

        // depth buffer
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        checkGlError("create RT Depth buffer");

        // attach texture to frame buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);
        // attach depth to frame buffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures[1], 0);
        checkGlError("attach texture and depth buffer to RT");

        RenderTarget* rt = 0;
        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status == GL_FRAMEBUFFER_COMPLETE)
        {
            LOGI("Frame buffer created. Handle = %d, width = %d, height = %d\n", handle[0], width, height);
            rt = HouyiNew RenderTarget(width, height);
            rt->setHandle(handle[0]);
            rt->setTextureId(textures[0]);
            rt->setDepthHandle(textures[1]);
            checkGlError("exit createRenderTarget");
        }
        else
        {
            LOGE("Frame buffer can not be created err = %d\n", status);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return rt;
    }
}
