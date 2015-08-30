#include "HouyiGLShadowPass.h"
#include "HouyiGLUtils.h"
#include "HouyiGLESRenderer.h"
#include "HouyiRoot.h"

namespace Houyi
{
    GLShadowPass::GLShadowPass(int program) : GLPass(program), mShadowRT(0), mDebugShadow(false)
    {
        mName = "Shadow Pass";
    }

    GLShadowPass::GLShadowPass(const char* vertex, const char* fragment) : GLPass(vertex, fragment)
    {
        mName = "Shadow Pass";
    }

    GLShadowPass::~GLShadowPass()
    {
        if (mShadowRT)
        {
            HouyiDelete(mShadowRT);
            mShadowRT = 0;
        }
    }

    void GLShadowPass::onRender(Scene* scene)
    {
        if (scene->getLightCount() <= 0)
        {
            return;
        }

        Root* root = Root::getInstance();
        // since this is GL shadow pass, we can cast renderer to GLESRenderer
        GLESRenderer* renderer = (GLESRenderer*)root->getRenderer();

        if (!mShadowRT)
        {
            float width = renderer->getWidth();
            float height = renderer->getHeight();

            mShadowRT = GLUtils::createRenderTarget(width, height);

            Light* light = scene->getLight(0);// TODO multiple shadow
            float* pos = light->getPosition();
            Camera shadowCamera(Vector3(pos[0]*2, pos[1]*2, pos[2]*2), Vector3(0, 0, 0), 0, width / height);
            shadowCamera.setFOV(120);
            shadowCamera.perspective();
            Matrix4 view = shadowCamera.getViewMatrix();
            light->setShadowCastViewMatrix(view);
            Matrix4& m4 = shadowCamera.getProjectionMatrix();
            light->setShadowCastProjectionwMatrix(m4);
        }

        if (mDebugShadow)
        {
            mPassState = EDebug;
        }
        else
        {
            renderer->bindRenderTarget(mShadowRT);
        }

        CHECK_GL_ERROR("shadow pass start");

        glEnable(GL_DEPTH_TEST);

        glClearColor(1, 1, 1, 1);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        Light* light = scene->getLight(0); // TODO
        int locMat = glGetUniformLocation(mProgram, "matLightView");
        glUniformMatrix4fv(locMat, 1, false, light->getShadowCastViewMatrix().getData());
        int locProMat = glGetUniformLocation(mProgram, "matLightProjection");
        glUniformMatrix4fv(locProMat, 1, false, light->getShadowCastProjectionwMatrix().getData());

        for (int i = 0;i < scene->getMeshCount();++i)
        {
            Mesh* mesh = scene->getMesh(i);
            if (mesh->isShadowCastEnabled())
            {
                renderer->uploadBone(mesh);
                SurfacePtr surface = mesh->getShadowSurface();
                if (surface)
                {
                    renderer->renderMeshSurface(surface, false);
                }
                else
                {
                    for (int i = 0; i < mesh->getSurfaceCount(); ++i)
                    {
                        surface = mesh->getSurface(i);
                        renderer->renderMeshSurface(surface, false);
                    }
                }
            }
        }

        mMyResult.mTextureIds.clear();
        mMyResult.mTextureIds.push_back(mShadowRT->getTextureId());

        if (!mDebugShadow)
        {
            renderer->bindRenderTarget(0);
        }

        CHECK_GL_ERROR("shadow pass end");
    }
}
