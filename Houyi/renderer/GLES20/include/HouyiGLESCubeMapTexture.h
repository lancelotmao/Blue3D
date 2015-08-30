#ifndef __Houyi__HouyiGLESCubeMapTexture__
#define __Houyi__HouyiGLESCubeMapTexture__

#include "HouyiGLESTexture.h"

namespace Houyi
{
    class GLESCubeMapTexture : public GLESTexture
    {
    public:
        GLESCubeMapTexture(ImagePtr images[6], bool upload = true);
        virtual ~GLESCubeMapTexture();
        virtual void uploadTexture();
        virtual void uploadTexSubImage2D();
        virtual void uploadTexSubImage2D(ImagePtr bitmap, int x, int y, int width, int height);
        virtual void uploadTexSubImage2D(void* data, int x, int y, int width, int height);
        virtual void allocTexMemory(){}

    private:
        ImagePtr mImages[6];
    };
}

#endif
