//
//  HouyiHeavenlyObject.cpp
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiHeavenlyObject.h"
#include "HouyiGeoSphere.h"
#include "HouyiImageFactory.h"
#include "HouyiTextureManager.h"
#include "HouyiSceneManager.h"
#include "HouyiRoot.h"
#include "HouyiWorld.h"
namespace Houyi
{
    HeavenlyObject::HeavenlyObject()
    {
//        Root* root = Root::getInstance();
//        World* world = root->getWorld();
//        Scene* scene = world->getFocusScene();
//
//        Mesh* mesh = HouyiNew GeoSphere(1, 5, 10);
//        scene->addMesh(mesh);
//        mBody = HouyiNew SceneNode();
//        mBody->setMesh(mesh);
//        
//        mMaterial = HouyiNew Material();
//        mBody->addMaterial(mMaterial);
//        
//        addChild(mBody);
    }
    
    HeavenlyObject::~HeavenlyObject()
    {
        HouyiDelete(mMaterial);
    }
    
    void HeavenlyObject::setTextureImage(const string& path)
    {
        setTextureImage(ImageFactory::createImage(path));
    }
    
    void HeavenlyObject::setTextureImage(ImagePtr img)
    {
        if (img)
        {
            mMaterial->clearTexture();
//            mMaterial->addTexture(TextureManager::getInstance()->createTexture(img));
        }
    }
}
