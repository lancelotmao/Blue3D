//
//  HouyiSphereEnvMap.h
//  Houyi
//
//  Created by Lance Mao on 6/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiSphereEnvMap_h
#define Houyi_HouyiSphereEnvMap_h

#include "HouyiSceneNode.h"
#include "HouyiCamera.h"

namespace Houyi
{
    class Scene;
    class SphereEnvMap : public SceneNode
    {
    public:
        SphereEnvMap(Scene* scene, const string textureName);
        
        inline Material* getMaterial()
        {
            return mMaterial;
        }
        
        virtual void onRenderBegin(Pass* pass);
        virtual void onRenderEnd(Pass* pass);
        
    private:
        Material* mMaterial;
    };
}

#endif
