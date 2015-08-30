//
//  HouyiHeavenlyObject.h
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiHeavenlyObject_h
#define Houyi_HouyiHeavenlyObject_h

#include "HouyiSceneNode.h"

namespace Houyi
{
    class HeavenlyObject : public SceneNode
    {
    public:
        HeavenlyObject();
        virtual ~HeavenlyObject();
        
        inline Material* getMaterial()
        {
            return mMaterial;
        }
        
        virtual void setTextureImage(const string& path);
        virtual void setTextureImage(ImagePtr img);
        
    protected:
        SceneNode* mBody;
        Material* mMaterial;
    };
}

#endif
