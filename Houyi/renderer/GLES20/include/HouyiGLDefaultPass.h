#ifndef __Houyi__GLDefaultPass__
#define __Houyi__GLDefaultPass__

#include "HouyiGLPass.h"

namespace Houyi
{
    class GLDefaultPass : public GLPass
    {
    public:
        GLDefaultPass(int program);
        GLDefaultPass(const char* vertex, const char* fragment);
        virtual ~GLDefaultPass();

    protected:
        virtual void onRender(Scene* scene);

        virtual bool linearizeZ();

    private:
        float mTick;
    };
}

#endif
