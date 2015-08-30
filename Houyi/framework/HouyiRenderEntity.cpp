//
//  HouyiRenderEntity.cpp
//  Houyi
//
//  Created by Lance Mao on 12/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiRenderEntity.h"
#include "HouyiSceneNode.h"

namespace Houyi
{
    const RenderEntity RenderEntity::NullEntity = RenderEntity(0, 0, 0);
    
    RenderEntity::RenderEntity(SceneNode* node, SurfacePtr surface, Material* material) :
    mNode(node), mSurface(surface), mMaterial(material)
    {
        
    }

    bool RenderEntity::belongsToSceneNode(SceneNode* node) const
    {
        SceneNode* p = mNode;
        while(p)
        {
            if (p == node)
            {
                return true;
            }
            p = (SceneNode*)(p->getParent());
        }
        return false;
    }
}
