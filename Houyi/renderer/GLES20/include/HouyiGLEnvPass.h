#ifndef __Houyi__GLEnvPass__
#define __Houyi__GLEnvPass__

#include "HouyiGLPass.h"

namespace Houyi
{
    class GLEnvPass : public GLPass
    {
    public:
        GLEnvPass(int program);
        GLEnvPass(const char* vertex, const char* fragment);
        virtual ~GLEnvPass();

    protected:
        virtual void onRender(Renderer* renderer, Scene* scene);

    private:
    };
}

#endif
