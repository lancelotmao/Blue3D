#include "HouyiPlanet.h"
#include "HouyiGeoPlane.h"
#include "HouyiCircle.h"
#include "HouyiCircleOutline.h"
#include "HouyiRing.h"
#include "HouyiSceneManager.h"
#include "HouyiImageFactory.h"
#include "HouyiCanvasFactory.h"
#include "HouyiTextureManager.h"
#include "HouyiMaterialManager.h"
#include "HouyiRoot.h"
#include "HouyiWorld.h"

namespace Houyi
{
    Planet::Planet(string name) : mDistance(1), mDistanceSim(1), mAngle(0),
            mAngularSpeed(0), mAngularSpeedSim(0), mSize(1), mSizeSim(1), mRing(0)
    {
        mName = name;
        Root* root = Root::getInstance();
        World* world = root->getWorld();
        Scene* scene = world->getFocusScene();
        
        Mesh* mesh = HouyiNew CircleOutline(1, 1);
        VertexBufferPtr vb = mesh->getSurface(0)->getVertexBuffer();
        int vcnt = vb->getVertexCount();
        float* color = new float[vcnt * 4];
        for (int i = 0;i < vcnt;++i)
        {
            color[i * 4] = 0.1137;
            color[i * 4 + 1] = 0.2117;
            color[i * 4 + 2] = 0.3019;
            color[i * 4 + 3] = 1 - i / (float)(vcnt-1);
        }
        vb->setColor(color, vcnt * 4);
        scene->addMesh(mesh);
        mOrbit = HouyiNew SceneNode();
        mOrbit->setMesh(mesh);
        mOrbit->rotate(1, 0, 0, Math::HALF_PI);
        mOrbit->setName(mName + " Orbit");
        
        Material* mat = HouyiNew Material();
        scene->getMaterialManager()->addMaterial(mat);
        mat->setDiffuseColor(0);
        mat->setTransparentMode(Material::EView);
        mat->getRenderState().disableLighting();
        mOrbit->addMaterial(mat);
        mOrbit->setZOrder(-1);
        addChild(mOrbit);
        
        mNodeName = HouyiNew SceneNode();
        mesh = HouyiNew GeoPlane(1, 1, 2, 1, false, true);
        scene->addMesh(mesh);
        mNodeName->setMesh(mesh);
        ImagePtr img = ImageFactory::createImage(128, 64);
        CanvasPtr c = CanvasFactory::getCanvas(img);
        c->setFontSize(18);
        c->setColor(0xffffffff);
        c->drawString(mName, 64, 32, Canvas::EHAlignCenter, Canvas::EVAlignCenter);
        
        mat = HouyiNew Material();
        scene->getMaterialManager()->addMaterial(mat);
        mat->getRenderState().disableLighting();
        mat->getRenderState().disableDepthTest();
        mat->addTexture(TextureManager::getInstance()->createTexture(img));
        mNodeName->addMaterial(mat);
        mNodeName->setZOrder(1000);
        addChild(mNodeName);
    }
    
    void Planet::addRing(float innerR, float outterR, string texName)
    {
        Root* root = Root::getInstance();
        World* world = root->getWorld();
        Scene* scene = world->getFocusScene();

        Mesh* mesh = HouyiNew Ring(innerR, outterR, 10);
        scene->addMesh(mesh);
        mRing = HouyiNew SceneNode();
        mRing->setName(mName + " Ring");
        mRing->setMesh(mesh);
        Material* mat = HouyiNew Material();
        scene->getMaterialManager()->addMaterial(mat);
        Texture* t = TextureManager::getInstance()->createTexture(scene, texName);
        t->setWrapMode(Texture::WRAP_CLAMP, Texture::WRAP_CLAMP);
        mat->addTexture(t);
        mRing->addMaterial(mat);
        mRing->setZOrder(100);

        addChild(mRing);
    }

    bool Planet::onUpdate()
    {
        mAngle -= mAngularSpeedSim;
        Vector3 bodyPos(0, 0, -mDistanceSim);
        bodyPos.rotate(Vector3(0, 1, 0), -mAngle);

        mBody->setIdentity();
        mBody->scale(mSizeSim);
        mBody->translate(0, 0, -mDistanceSim);
        mBody->rotate(0, 1, 0, mAngle);
        
        mOrbit->setIdentity();
        mOrbit->scale(mDistanceSim);
        mOrbit->rotate(1, 0, 0, -Math::HALF_PI);
        mOrbit->rotate(0, 1, 0, mAngle-Math::HALF_PI);
        
        mNodeName->setIdentity();
        mNodeName->translate(0, 0.5, 0);
        mNodeName->scale(4);
        mNodeName->translate(bodyPos);
        mNodeName->translate(0, mSizeSim, 0);
        
        if (mRing)
        {
            mRing->setIdentity();
            mRing->scale(mSizeSim);
            mRing->rotate(1, 0, 0, -Math::HALF_PI);
            mRing->translate(bodyPos);
        }
        return false;
    }
}
