#ifndef Houyi_HouyiAxis_h
#define Houyi_HouyiAxis_h

#include "HouyiSceneNode.h"
#include "HouyiCylinder.h"
#include "HouyiCone.h"

namespace Houyi
{
    class Scene;

    class Axis : public SceneNode
    {
    public:
        enum
        {
            ELeft, HCenter, ERight
        };
        
        enum
        {
            ETop, VCenter, EBottom
        };
        
        Axis();
        virtual ~Axis();

        void attachToScene(Scene* scene);
        virtual void onRenderBegin(Pass* pass);
        virtual void onRenderEnd(Pass* pass);
        
        inline void setPadding(float h, float v)
        {
            mHPadding = h;
            mVPadding = v;
        }

    private:
        float mWidth;
        float mHeight;
        int mAlignX;
        int mAlignY;
        float mHPadding;
        float mVPadding;
        Matrix4 mLastMatrix;
        
        Cylinder* mCylinder;
        Cone* mCone;
        SceneNode* mXCone;
        SceneNode* mYCone;
        SceneNode* mZCone;
        SceneNode* mXCylinder;
        SceneNode* mYCylinder;
        SceneNode* mZCylinder;
    };
}

#endif
