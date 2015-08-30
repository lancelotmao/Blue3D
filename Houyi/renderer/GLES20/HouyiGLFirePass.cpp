#include "HouyiGLFirePass.h"
#include "HouyiRoot.h"
#include "HouyiGLUtils.h"

namespace Houyi
{
    GLFirePass::GLFirePass(int program) : GLPass(program)
    {
        mName = "Fire Pass";
    }

    GLFirePass::GLFirePass(const char* vertex, const char* fragment) : GLPass(vertex, fragment)
    {
        mName = "Fire Pass";
    }

    GLFirePass::~GLFirePass()
    {

    }

    void GLFirePass::onRender(Scene* scene)
    {
//        GLPass::onRender(world);
//
//        static double maxTime = 16;
//        // time cannot growing forever, because in shader we (*time) rather than sin(time)
//        if (mDeltaTimeMillis > maxTime * 1000)
//        {
//            mStartTimeMillis = currentTimeMillis();
//        }
//
//        // TODO we cannot use linear, it will jump suddenly to the beginning
//        double dT = mDeltaTimeMillis/1000;
//
//        // work-around, seems that shader has some issue when time is near 0
//        double time_0_X = dT + 2;
//
//        Root* root = Root::getInstance();
//        Renderer* renderer = root->getRenderer();
//        RenderState& rs = renderer->getRenderState();
//
//        glEnable(GL_BLEND);
//        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//        glClearColor(0, 0, 0, 1);
//        glClear((GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT) & rs.getClearMask());
//        CHECK_GL_ERROR("glClear");
//
//        int locTime = glGetUniformLocation(mProgram, "time_0_X");
//        if (locTime >= 0)
//        {
//            glUniform1f(locTime, time_0_X);
//        }
//
//        int loc = glGetUniformLocation(mProgram, "sideFade");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 18.6);
//        }
//
//        loc = glGetUniformLocation(mProgram, "sideFadeSharpness");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 0.05);
//        }
//
//        loc = glGetUniformLocation(mProgram, "wobbleScale");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 0.03);
//        }
//
//        loc = glGetUniformLocation(mProgram, "burnSpeed");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 0.57);
//        }
//
//        loc = glGetUniformLocation(mProgram, "randomnessRate");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 0.24);
//        }
//
//        loc = glGetUniformLocation(mProgram, "yFade");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 0.6);
//        }
//
//        loc = glGetUniformLocation(mProgram, "xScale");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 1.5);
//        }
//
//        loc = glGetUniformLocation(mProgram, "yScale");
//        if (loc >= 0)
//        {
//            glUniform1f(loc, 0.6);
//        }
//
//        renderer->renderScene();
//
//        CHECK_GL_ERROR("Fire Pass end");
    }
}
