//
//  HouyiSphereEnvMap.cpp
//  Houyi
//
//  Created by Lance Mao on 6/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiSphereEnvMap.h"
#include "HouyiWorld.h"
#include "HouyiGeoSphere.h"
#include "HouyiMaterialManager.h"
#include "HouyiTextureManager.h"
#include "HouyiImageFactory.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"

namespace Houyi
{
    SphereEnvMap::SphereEnvMap(Scene* scene, const string textureName)
    {
        mName = "SpaceSphere";
        mSelectable = false;
        
        GeoSphere* s = HouyiNew GeoSphere(1, 30, 10);
        setMesh(s);
        scene->addMesh(s);
        
        mMaterial = scene->getMaterialManager()->createMaterial("sphereenvmap");
        ImagePtr img = ImageFactory::createImage(textureName);
        if (img)
        {
            mMaterial->clearTexture();
//            mMaterial->addTexture(TextureManager::getInstance()->createTexture(img));
        }
        mMaterial->getRenderState().disableLighting();
        mMaterial->getRenderState().setDoubleSided(true);
        mMaterial->getRenderState().disableDepthTest();
        s->setMaterial(mMaterial);
        
        scene->addSceneNode(this);
        scene->addRenderable(this);
    }
    
    void SphereEnvMap::onRenderBegin(Pass* pass)
    {
//        Camera* worldCamera = Root::getInstance()->getWorld()->getCurrentCamera();
//        if (!worldCamera)
//        {
//            return;
//        }
//        Camera cam;
//        cam.setAspectRatio(worldCamera->getAspectRatio());
//        cam.setUp(worldCamera->getUp());
//        cam.setFOV(worldCamera->getFOV());
//        
//        Vector3 f = worldCamera->getForward(true);
//        cam.setPosition(-f);
//        cam.setLookAt(f);
//        cam.lookAt();
//
//        Matrix4 m = cam.getViewMatrix();
//        pass->uploadEyeMat(m);
//
//        cam.setFOV(60);
//        cam.perspective();
//        pass->uploadProjectionMat(cam.getProjectionMatrix());
    }
    
    void SphereEnvMap::onRenderEnd(Pass* pass)
    {
//        Camera* worldCamera = Root::getInstance()->getWorld()->getCurrentCamera();
//        if (!worldCamera)
//        {
//            return;
//        }
//        pass->uploadEyeMat(worldCamera->getViewMatrix());
//        pass->uploadProjectionMat(worldCamera->getProjectionMatrix());
    }
}
