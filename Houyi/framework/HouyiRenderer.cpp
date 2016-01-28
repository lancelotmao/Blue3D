#include "HouyiRenderer.h"
#include "HouyiRoot.h"
#include "HIShaders.h"
#include "HouyiLog.h"
#include "HouyiUtil.h"
#include "HouyiTextureManager.h"
#include "HouyiCapability.h"

namespace Houyi
{
    Renderer::Renderer() :
            mWidth(0), mHeight(0), mCurrentRenderingScene(0),
            mIsMSAAEnabled(false), mMSAARenderTarget(0), mMSAAQuad(0), mMSAACamera(0), mMSAARequested(false),
            mSampleRate(1), mDefaultFBO(0), mScreenBufferScale(1), mIsTrilinearFilterEnabled(false),
            mVertexCount(0), mPointCount(0), mLineCount(0), mTriangleCount(0), mCurrentPass(0), mLastPass(0), mSingleViewPort(true), mShaderMan(0)
    {
    }

    Renderer::~Renderer()
    {
        clearContext();
    }

    void Renderer::clearContext()
    {
        for (int i = 0; i < mRTs.size(); ++i)
        {
            RenderTarget* rt = mRTs.at(i);
            HouyiDelete(rt);
        }
        mRTs.clear();

        if (mMSAAQuad)
        {
            destroyRenderTarget(mMSAARenderTarget);
            HouyiDelete(mMSAAQuad); // delete also mMSAARenderTarget
            mMSAAQuad = 0;
        }

        mVertexCount = 0;
        mPointCount = 0;
        mLineCount = 0;
        mTriangleCount = 0;
        mActiveVB = 0;
    }

    Pass* Renderer::loadShader(int index)
    {
        LOGW("Calling super renderer to loadShader");
        return 0;
    }

    void Renderer::insertShadowPass()
    {
        Pass* pass = loadShader(HSHADER_SHADOW_CAST);
        mShaderMan->insertPass(pass, 0);
    }

    void Renderer::onWindowCreated()
    {
    	Capability::getGPUInfo();
    }

    void Renderer::onWindowChanged(int width, int height)
    {
    }

    bool Renderer::bindVertexBuffer(VertexBufferPtr vertexBuffer)
    {
        return false;
    }

    RenderTarget* Renderer::addRenderTarget(int width, int height)
    {
        RenderTarget* rt = createRenderTarget(width, height);
        if (rt)
        {
            mRTs.push_back(rt);
        }
        return rt;
    }

    RenderTarget* Renderer::createRenderTarget(int width, int height)
    {
        return 0;
    }

    void Renderer::destroyRenderTarget(RenderTarget* renderTarget)
    {
    }

    bool Renderer::enableMSAA(float sampleRate, bool enableTrilinearFilter)
    {
        mSampleRate = sampleRate;
        mIsTrilinearFilterEnabled = enableTrilinearFilter;
        if (mWidth == 0 || mHeight == 0)
        {
            mMSAARequested = true;
            return false;
        }

        int width = Util::roundPowerOf2(mWidth * mScreenBufferScale * mSampleRate);
        int height = Util::roundPowerOf2(mHeight * mScreenBufferScale * mSampleRate);
        LOGD("MSAA FBO width = %d, height = %d\n", width, height);
        if (mMSAARenderTarget)
        {
            destroyRenderTarget(mMSAARenderTarget);
            HouyiDelete(mMSAAQuad);
        }
        mMSAARenderTarget = createRenderTarget(width, height);
        if (mMSAARenderTarget)
        {
            mIsMSAAEnabled = true;
            mMSAARequested = false;
            mMSAAQuad = HouyiNew GeoPlane(1, 1, 2, 2, false, false);
            mMSAAQuad->generate(VertexBuffer::ENormal, VertexBuffer::Attribute_Texcoord,
                    Geometry::Generate_Triangle_Index);
            mMSAAQuad->setTexture(mMSAARenderTarget);

            mMSAAProgram = loadShader(HSHADER_MSAA)->getProgram();

            Matrix4& pjtMat = mMSAACamera.getProjectionMatrix();
            pjtMat = Matrix4::IDENTITY;
            pjtMat[0][0] = (float) mHeight / mWidth;
            mMSAACamera.getViewMatrix() = Matrix4::IDENTITY;

            return true;
        }
        else
        {
            LOGE("enableMSAA failed. Cannot create render target\n");
            return false;
        }
    }

    bool Renderer::isMSAAEnabled()
    {
        return mIsMSAAEnabled;
    }

    RenderTarget* Renderer::getMSAARenderTarget()
    {
        return mMSAARenderTarget;
    }

    void Renderer::setScreenBufferScale(float scale)
    {
        mScreenBufferScale = scale;
    }
    
    float Renderer::getScreenBufferScale()
    {
        return mScreenBufferScale;
    }

    void Renderer::bindRenderTarget(RenderTarget* rt)
    {
    }

    bool Renderer::renderAll(World* world)
    {
        if (world)
        {
            bool ret = beginRender(world);
            if (ret)
            {
                ret &= render(world);
            }
            ret &= endRender(world);
            return ret;
        }
        else
        {
            return false;
        }
    }

    bool Renderer::beginRender(World* world)
    {
        int passCount = mShaderMan->getPassCount();
        if (passCount == 0)
        {
            LogOnce("Renderer::beginRender. no pass\n");
            return false;
        }

        LogOnce("Pass Count: %d\n", passCount);
        string passInfo;
        for (int i = 0;i < passCount;++i)
        {
            Pass* p = mShaderMan->getPass(i);
            if (p)
            {
                passInfo += p->getName();
            }
            if (i < passCount - 1)
            {
                passInfo += ", ";
            }
        }
        LogOnce("%s\n", passInfo.c_str());

        mPointCount = 0;
        mLineCount = 0;
        mTriangleCount = 0;
        mDefaultFBO = getFrameBufferHandle();

        if (mIsMSAAEnabled)
        {
            bindRenderTarget(mMSAARenderTarget);
            Camera* cam = world->getCurrentCamera();
            if (cam)
            {
                cam->setAspectRatio(1);
            }
        }

        return true;
    }

    bool Renderer::render(World* world)
    {
        for (int s = world->getSceneCount() - 1;s >= 0;--s)
        {
            mCurrentRenderingScene = world->getScene(s);
            
            // check if we have pending texture upload request
            mCurrentRenderingScene->getTextureManager()->upload();
            
            if (mSingleViewPort)
            {
                mCurrentRenderingScene->getMaterialManager()->setLastBound(0, 0);
                // NOTE: camera is updated per scene
                // camera matrix is uploaded per render entity
                Camera* camera = mCurrentRenderingScene->getCurrentCamera();
                if (camera)
                {
                    mCurrentRenderingScene->setCurrentCamera(camera);
                    if (camera->getAspectRatio() != mWidth / mHeight)
                    {
                        camera->setAspectRatio(mWidth / mHeight);
                    }
                    camera->update();
                }
                
                renderSceneEntities(mCurrentRenderingScene);
                mLastPass = 0;
            }
            else
            {
                for (int v = 0;v < mCurrentRenderingScene->mViewPorts.size();v++)
                {
                    // last bound material is a per scene attribute
                    mCurrentRenderingScene->getMaterialManager()->setLastBound(0, 0);
                    
                    ViewPort& vp = mCurrentRenderingScene->mViewPorts[v];
                    
                    setViewport(vp.mBound.mLeft, vp.mBound.mTop, vp.mBound.mWidth, vp.mBound.mHeight);
                    
                    // NOTE: camera is updated per scene
                    // camera matrix is uploaded per render entity
                    Camera* camera = vp.mCamera;
                    if (camera)
                    {
                        mCurrentRenderingScene->setCurrentCamera(camera);
                        if (camera->getAspectRatio() != vp.mBound.mWidth / vp.mBound.mHeight)
                        {
                            camera->setAspectRatio(vp.mBound.mWidth / vp.mBound.mHeight);
                        }
                        camera->update();
                    }
                    
                    renderSceneEntities(mCurrentRenderingScene);
                    mLastPass = 0;
                }
            }
        }
        return true;
    }

    bool Renderer::endRender(World* world)
    {
        mShaderMan->endRender(mCurrentRenderingScene);
        mCurrentPass = 0;
        mLastPass = 0;

        if (mIsMSAAEnabled)
        {
            bindRenderTarget(0);
            renderMSAAQuad();
        }

        if (mCurrentRenderingScene->getTextureManager()->hasPendingTexture())
        {
            world->getRoot()->requestRender();
        }
        return true;
    }

    bool Renderer::renderScene()
    {
        return true;
    }
    
    bool Renderer::renderSceneEntities(Scene* scene)
    {
        for (int i = 0, n = scene->getRenderableCount(); i < n;++i)
        {
            const RenderEntity& entity = scene->getRenderable(i);
            SceneNode* node = entity.mNode;
            if (!node->isVisible())
            {
                continue;
            }
            
            Material* mat = entity.mMaterial;
            if (mat)
            {
                Pass* pass = mat->getPass();
                if (!pass)
                {
                    pass = mShaderMan->getDefaultShader(this);
                    mat->setPass(pass);
                }
                if (pass && mLastPass != pass)
                {
                    mCurrentPass = pass;
                    pass->beginPass();
                    pass->renderPass(this, scene);
                }
            }
            else
            {
                continue;
            }
            
            uploadCamera(mCurrentPass, scene);
            
            SceneNodeRenderListener* listener = node->getRenderListener();
            if (listener)
            {
                listener->onRenderBegin(mat->getPass());
            }
            
            renderSceneEntity(entity);
            
            if (listener)
            {
                listener->onRenderEnd(mat->getPass());
            }
            
            if (mat)
            {
                Pass* pass = mat->getPass();
                if (pass && mLastPass != pass)
                {
                    pass->endPass();
                    mLastPass = pass;
                }
            }
        }
        return true;
    }

    bool Renderer::renderSceneEntity(const RenderEntity& entity)
    {
        return false;
    }

    void Renderer::onPassBegin(const Pass* pass)
    {
    }

    void Renderer::onPassEnd(const Pass* pass)
    {
    }

    void Renderer::renderMSAAQuad()
    {
        // TODO wrap with node make sure add mesh to world to prevent leak
//        renderMesh(mMSAAQuad);
    }

    void Renderer::uploadBone(Mesh* mesh)
    {
    }
    
    void Renderer::uploadCamera(Pass* pass, Scene* scene)
    {
        Camera* camera = scene->getCurrentCamera();
        if (camera)
        {    
            Matrix4 viewMatrix = camera->getViewMatrix();
            pass->uploadEyeMat(viewMatrix);
            if (pass->linearizeZ())
            {
                Matrix4 p = camera->getProjectionMatrix();
                float far = camera->getFarRange();
                p[2][2] /= far;
                p[2][3] /= far;
                pass->uploadProjectionMat(p);
            }
            else
            {
                pass->uploadProjectionMat(camera->getProjectionMatrix());
            }
            
            int locEyePos = pass->getUniformLocation("uEyePosition");
            if (locEyePos >= 0)
            {
                const Vector3& pos = camera->getPosition();
                glUniform3f(locEyePos, pos.x, pos.y, pos.z);
            }
        }
    }

    int Renderer::getVertexCount()
    {
        if (mCurrentRenderingScene)
        {
            return mCurrentRenderingScene->getStats().mVertexCount;
        }
        return mVertexCount;
    }

    int Renderer::getPointCount()
    {
        return mPointCount;
    }

    int Renderer::getLineCount()
    {
        if (mCurrentRenderingScene)
        {
            return mCurrentRenderingScene->getStats().mLineCount;
        }
        return mLineCount;
    }

    int Renderer::getTriangleCount()
    {
        if (mCurrentRenderingScene)
        {
            return mCurrentRenderingScene->getStats().mTriangleCount;
        }
        return mTriangleCount;
    }
}
