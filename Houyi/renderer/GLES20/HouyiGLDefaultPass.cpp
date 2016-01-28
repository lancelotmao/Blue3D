#include "HouyiGLDefaultPass.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"
#include "HouyiGLUtils.h"
#include "HouyiSettings.h"

namespace Houyi
{
    GLDefaultPass::GLDefaultPass(int program) : GLPass(program)
    {
        mName = "Default Pass";
    }

    GLDefaultPass::GLDefaultPass(const char* vertex, const char* fragment) : GLPass(vertex, fragment)
    {
        mName = "Default Pass";
    }

    GLDefaultPass::~GLDefaultPass()
    {

    }

    void GLDefaultPass::onRender(Renderer* renderer, Scene* scene)
    {
        CHECK_GL_ERROR("Default Pass start");

        GLPass::onRender(renderer, scene);

        if (!renderer)
        {
            LOGE("GLDefaultPass::onRender. Renderer is NULL");
            return;
        }
        float width = renderer->getWidth();
        float height = renderer->getHeight();

        int locTick = getUniformLocation("uTick");
        if (locTick >= 0)
        {
            glUniform1f(locTick, mTick);
            mTick += 0.1f;
            if (mTick > Math::PI * 8)
            {
                mTick = 0;
            }
        }

        int locWidth = getUniformLocation("width");
        if (locWidth >= 0)
        {
            glUniform1f(locWidth, width);
        }

        int locHeight = getUniformLocation("height");
        if (locHeight >= 0)
        {
            glUniform1f(locHeight, height);
        }

        renderer->renderScene();

        CHECK_GL_ERROR("Default Pass end");
    }

    bool GLDefaultPass::linearizeZ()
    {
        return false;
    }
}
