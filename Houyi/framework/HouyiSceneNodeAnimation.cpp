//
//  SceneNodeAnimation.cpp
//  Houyi
//
//  Created by Lance Mao on 1/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiSceneNodeAnimation.h"

namespace Houyi
{
    void SceneNodeAnimation::onAnimation(double ratio)
    {
        if (!mNode)
        {
            return;
        }
        
        Vector3 srcS;
        Quaternion srcR;
        Vector3 srcT;
        mSource.decomposition(srcT, srcS, srcR);

        Vector3 tgtS;
        Quaternion tgtR;
        Vector3 tgtT;
        mTarget.decomposition(tgtT, tgtS, tgtR);
        
        Vector3 s = srcS + (tgtS - srcS) * ratio;
        Quaternion q = Quaternion::Slerp(ratio, srcR, tgtR, true);
        Vector3 pos = srcT + (tgtT - srcT) * ratio;
        
        Matrix4 m;
        m.makeTransform(pos, s, q);
        mNode->setLocal(m);
    }
    
    void SceneNodeAnimation::onAnimationStop()
    {
    }
    
    void SceneNodeAnimation::save()
    {
        if (mNode)
        {
            mSource = mNode->getLocal();
        }
    }
    
    void SceneNodeAnimation::setIdentityAsTarget()
    {
        mTarget = Matrix4::IDENTITY;
    }
}
