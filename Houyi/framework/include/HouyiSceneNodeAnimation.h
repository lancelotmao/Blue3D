//
//  SceneNodeAnimation.h
//  Houyi
//
//  Created by Lance Mao on 1/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_SceneNodeAnimation_h
#define Houyi_SceneNodeAnimation_h

#include "HouyiAnimation.h"
#include "HouyiSceneNode.h"
#include "HouyiMatrix4.h"

namespace Houyi
{
    class SceneNodeAnimation : public Animation, public AnimationListener
    {
    public:
        SceneNodeAnimation() :
        mNode(0), mSource(Matrix4::IDENTITY), mTarget(Matrix4::IDENTITY)
        {
            setAnimationListener(this);
        }
        
        inline void setNode(SceneNode* node)
        {
            mNode = node;
        }
        
        void setSource(Matrix4 src)
        {
            mSource = src;
        }
        
        void setTarget(Matrix4 target)
        {
            mTarget = target;
        }
        
        // set current transform as source
        void save();
        
        // use identity as target
        void setIdentityAsTarget();
        
        void onAnimationStop();
        
    protected:
        virtual void onAnimation(double ratio);
        
    private:
        SceneNode* mNode;
        
        Matrix4 mSource;
        Matrix4 mTarget;
    };
}
#endif
