#ifndef __Houyi__GLFirePass__
#define __Houyi__GLFirePass__

#include "HouyiGLPass.h"

namespace Houyi
{
    class GLFirePass : public GLPass
    {
    public:
        GLFirePass();
        GLFirePass(int program);
        GLFirePass(const char* vertex, const char* fragment);
        virtual ~GLFirePass();

    protected:
        virtual void onRender(Scene* scene);

    private:
    };
}

#endif
