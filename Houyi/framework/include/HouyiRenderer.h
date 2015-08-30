#ifndef Houyi_HouyiRenderer_h
#define Houyi_HouyiRenderer_h

#include <vector>
#include "HouyiObject.h"
#include "HouyiRenderState.h"
#include "HouyiWorld.h"
#include "HouyiRenderTarget.h"
#include "HouyiGeoPlane.h"
#include "HouyiPass.h"
#include "HouyiRenderEntity.h"
#include "HouyiShaderManager.h"

namespace Houyi
{

    class Renderer: public Object, PassListener
    {
    public:
        Renderer();
        virtual ~Renderer();
        virtual void clearContext();
        
        virtual Pass* loadShader(int index);
        virtual Pass* loadShader(const char* pVertexSource, const char* pFragmentSource) = 0;
        virtual bool uploadShader(Pass* pass) = 0;
        
        virtual void onWindowCreated();
        virtual void onWindowChanged(int width, int height);

        inline float getWidth()
        {
            return mWidth;
        }

        inline float getHeight()
        {
            return mHeight;
        }

        inline void setWidth(float width)
        {
            mWidth = width;
        }

        inline void setHeight(float height)
        {
            mHeight = height;
        }
        
        inline int getProgram()
        {
            return mCurrentPass ? mCurrentPass->getProgram() : -1;
        }
        
        inline void setSingleViewPort(bool isSingle)
        {
            mSingleViewPort = isSingle;
        }
        
        inline bool isSingleViewPort()
        {
            return mSingleViewPort;
        }

        virtual void setViewport(float left, float top, float width, float height){}
        
        virtual bool bindVertexBuffer(VertexBufferPtr vertexBuffer);
        // delete hardware buffer from GPU to avoid leak
        // note that when context is lost, system will delete buffers automatically
        virtual void deleteHardwareBuffer(int count, uint* handle){}
        
        virtual RenderTarget* addRenderTarget(int width, int height);
        virtual void bindRenderTarget(RenderTarget* rt);
        virtual bool enableMSAA(float sampleRate, bool enableTrilinearFilter = false);
        virtual bool isMSAAEnabled();
        virtual RenderTarget* getMSAARenderTarget();
        virtual void setScreenBufferScale(float scale);
        virtual float getScreenBufferScale();

        // one blow
        virtual bool renderAll(World* world);

        // divide task
        virtual bool beginRender(World* world);
        virtual bool render(World* world);
        virtual bool endRender(World* world);

        virtual bool renderScene();
        virtual bool renderSceneEntities(Scene* scene);
        virtual bool renderSceneEntity(const RenderEntity& entity);

        // handle multi-pass
        virtual void onPassBegin(const Pass* pass);
        virtual void onPassEnd(const Pass* pass);
        virtual void insertShadowPass();

        virtual void uploadBone(Mesh* mesh);
        virtual void uploadCamera(Pass* pass, Scene* scene);

        // statistics
        virtual int getVertexCount();
        virtual int getPointCount();
        virtual int getLineCount();
        virtual int getTriangleCount();

    protected:
        virtual RenderTarget* createRenderTarget(int width, int height);
        virtual void destroyRenderTarget(RenderTarget* renderTarget);
        virtual void renderMSAAQuad();

    protected:
        //debug info
        int mVertexCount;
        int mPointCount;
        int mLineCount;
        int mTriangleCount;

        int mDefaultFBO;
        float mScreenBufferScale; // e.g. retina display will be 2
        float mWidth;
        float mHeight;
        Scene* mCurrentRenderingScene; // set at beginRender

        bool mIsMSAAEnabled;
        bool mIsTrilinearFilterEnabled;
        bool mMSAARequested;
        float mSampleRate;
        int mMSAAProgram;
        RenderTarget* mMSAARenderTarget;
        GeoPlane* mMSAAQuad;
        Camera mMSAACamera;
        std::vector<RenderTarget*> mRTs;

        VertexBufferPtr mActiveVB;
        Pass* mCurrentPass;
        Pass* mLastPass;
        ShaderManager* mShaderMan;
        
        bool mSingleViewPort;
    };
}

#endif
