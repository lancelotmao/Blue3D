#ifndef __Houyi__HouyiGLESTexture__
#define __Houyi__HouyiGLESTexture__

#include "HouyiTexture.h"

namespace Houyi
{
    class GLESTexture: public Texture
    {
    public:
        GLESTexture()
        {
        }
        GLESTexture(ImagePtr image, TextureConfig config = TextureConfig::DEFAULT);
        GLESTexture(int tid);
        virtual ~GLESTexture();

        virtual void setImage(ImagePtr image);
        virtual void setWrapMode(int s, int t);
        virtual void uploadTexture();
        virtual void allocTexMemory();
        virtual void uploadTexSubImage2D();
        virtual void uploadTexSubImage2D(ImagePtr bitmap, int x, int y, int width, int height);
        virtual void uploadTexSubImage2D(void* data, int x, int y, int width, int height);

    protected:
        GLuint mTextureIds[1];
        TextureConfig mConfig;

    protected:
        virtual void ensurePOT();
        virtual void glGenerate();
    };
}

#endif
