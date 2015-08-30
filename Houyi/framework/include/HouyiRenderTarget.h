#ifndef Houyi_HouyiRenderTarget_h
#define Houyi_HouyiRenderTarget_h

#include "HouyiTexture.h"

namespace Houyi
{
    class RenderTarget: public Texture
    {
    public:
        RenderTarget(int width, int height)
        {
            mWidth = width;
            mHeight = height;
        }

        ~RenderTarget()
        {
        }

    public:
        inline void setHandle(int handle)
        {
            mHandle = handle;
        }

        inline int getHandle()
        {
            return mHandle;
        }

        inline void setDepthHandle(int handle)
        {
            mDepthHandle = handle;
        }

        inline int getDepthHandle()
        {
            return mDepthHandle;
        }

    private:
        int mHandle;
        int mDepthHandle;
    };
}
#endif
