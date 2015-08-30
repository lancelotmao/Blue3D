#ifndef Houyi_HouyiCameraNode_h
#define Houyi_HouyiCameraNode_h

#include "HouyiSceneNode.h"
#include "HouyiGeoPlane.h"

namespace Houyi
{
    class Scene;

    // confusing enough this is not GL camera it is hardware camera on mobile phone
    class CameraNode : public SceneNode
    {
    public:
        CameraNode();
        virtual ~CameraNode();

        void attachToScene(Scene* scene);
        virtual void onRenderBegin(Pass* pass);
        virtual void onRenderEnd(Pass* pass);

        inline void setTextureId(int tid)
        {
            if (mMaterial)
            {
                mMaterial->setExternTexID(tid);
            }
        }

        inline void setPadding(float h, float v)
        {
            mHPadding = h;
            mVPadding = v;
        }

        inline void setPreviewSize(float w, float h)
        {
            mPreviewWidth = w;
            mPreviewHeight = h;
        }

    private:
        float mPreviewWidth;
        float mPreviewHeight;
        int mAlignX;
        int mAlignY;
        float mHPadding;
        float mVPadding;
        Material* mMaterial;
    };
}

#endif
