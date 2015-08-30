#ifndef __Houyi__GLShadowPass__
#define __Houyi__GLShadowPass__

#include "HouyiGLPass.h"
#include "HouyiCamera.h"
#include "HouyiRenderTarget.h"

namespace Houyi
{
    class GLShadowPass : public GLPass
    {
    public:
        GLShadowPass(int program);
        GLShadowPass(const char* vertex, const char* fragment);
        virtual ~GLShadowPass();

        virtual void onRender(Scene* scene);

    private:
        RenderTarget* mShadowRT;
        bool mDebugShadow;
    };
}

#endif
