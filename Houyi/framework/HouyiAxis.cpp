#include "HouyiAxis.h"
#include "HouyiRenderer.h"
#include "HouyiPlane.h"
#include "HouyiScene.h"
#include "HouyiPass.h"
#include "HouyiRoot.h"
#include "HouyiMeshOptimizer.h"

namespace Houyi
{
    Axis::Axis() : mAlignX(ELeft), mAlignY(EBottom), mHPadding(0), mVPadding(0.2)
    {
        setName("Axis");
        
        float cylinderHeight = 0.1;
        mCylinder = HouyiNew Cylinder(0.005, cylinderHeight, 120);
        mCone = HouyiNew Cone(0.01, 0.02, 120);
        MeshOptimizer::generateWireFrame(mCylinder);
        MeshOptimizer::generateWireFrame(mCone);

        float ratio = 0.15;
        mHeight = 2 * ratio; // GL Space Height is 2
        mWidth = mHeight;
        
        SceneNode* xAxis = HouyiNew SceneNode();
        mXCylinder = HouyiNew SceneNode();
        mXCylinder->setMesh(mCylinder);
        mXCylinder->setRenderListener(this);
        xAxis->addChild(mXCylinder);
        mXCone = HouyiNew SceneNode();
        mXCone->setMesh(mCone);
        mXCone->setRenderListener(this);
        mXCone->translate(Vector3(0, cylinderHeight, 0));
        xAxis->addChild(mXCone);
        xAxis->rotate(0, 0, 1, Math::PI / 2);
        addChild(xAxis);
        
        SceneNode* yAxis = HouyiNew SceneNode();
        mYCylinder = HouyiNew SceneNode();
        mYCylinder->setMesh(mCylinder);
        mYCylinder->setRenderListener(this);
        yAxis->addChild(mYCylinder);
        mYCone = HouyiNew SceneNode();
        mYCone->setMesh(mCone);
        mYCone->setRenderListener(this);
        mYCone->translate(Vector3(0, cylinderHeight, 0));
        yAxis->addChild(mYCone);
        addChild(yAxis);
        
        SceneNode* zAxis = HouyiNew SceneNode();
        mZCylinder = HouyiNew SceneNode();
        mZCylinder->setMesh(mCylinder);
        mZCylinder->setRenderListener(this);
        zAxis->addChild(mZCylinder);
        mZCone = HouyiNew SceneNode();
        mZCone->setMesh(mCone);
        mZCone->setRenderListener(this);
        mZCone->translate(Vector3(0, cylinderHeight, 0));
        zAxis->addChild(mZCone);
        zAxis->rotate(1, 0, 0, -Math::PI / 2);
        addChild(zAxis);
    }

    Axis::~Axis()
    {
    }

    void Axis::attachToScene(Scene* scene)
    {
        scene->addSceneNode(this);

        scene->addMesh(mCylinder);
        scene->addMesh(mCone);
        
        mXCylinder->setColor(scene, Color(0xFFFF0000));
        mXCylinder->getMaterial(0)->setPassConfigurable(false);
        scene->addRenderable(mXCylinder);
        
        mXCone->setColor(scene, Color(0xFFFF0000));
        mXCone->getMaterial(0)->setPassConfigurable(false);
        scene->addRenderable(mXCone);
        
        mYCylinder->setColor(scene, Color(0xFF00FF00));
        mYCylinder->getMaterial(0)->setPassConfigurable(false);
        scene->addRenderable(mYCylinder);
        
        mYCone->setColor(scene, Color(0xFF00FF00));
        mYCone->getMaterial(0)->setPassConfigurable(false);
        scene->addRenderable(mYCone);
        
        mZCylinder->setColor(scene, Color(0xFF0000FF));
        mZCylinder->getMaterial(0)->setPassConfigurable(false);
        scene->addRenderable(mZCylinder);
        
        mZCone->setColor(scene, Color(0xFF0000FF));
        mZCone->getMaterial(0)->setPassConfigurable(false);
        scene->addRenderable(mZCone);
    }

    void Axis::onRenderBegin(Pass* pass)
    {
        Scene* scene = getScene();
        Matrix4 m;
        if (scene)
        {
            m = scene->getWorld()->getCurrentCamera()->getViewMatrix();
            Renderer* renderer = scene->getWorld()->getRoot()->getRenderer();
            float sw = renderer->getWidth();
            float sh = renderer->getHeight();
            float aspect = sw / sh;
            float vpadding = mVPadding;
            float hpadding = mHPadding;
            if (mAlignX == ELeft)
            {
                m[0][3] = -aspect + mWidth / 2 + hpadding;
            }
            else if (mAlignX == ERight)
            {
                m[0][3] = aspect - mWidth / 2 - hpadding;
            }
            
            if (mAlignY == EBottom)
            {
                m[1][3] = -1 + mHeight / 2 + vpadding;
            }
            else if (mAlignY == ETop)
            {
                m[1][3] = 1 - mHeight / 2 - vpadding;
            }
            m[2][3] = -1 / tan(15 * Math::PI / 180);
            mLastMatrix = m;
        }
        else
        {
            m = mLastMatrix;
        }
        
        pass->uploadEyeMat(m);

        if (scene && scene->getCurrentCamera())
        {
            Camera* worldCamera = scene->getCurrentCamera();
            Camera cam;
            cam.setAspectRatio(worldCamera->getAspectRatio());
            cam.setFOV(worldCamera->getFOV());
            cam.setNearRange(1);
            cam.setFarRange(100);
            cam.perspective();
            pass->uploadProjectionMat(cam.getProjectionMatrix());
        }
    }

    void Axis::onRenderEnd(Pass* pass)
    {
    	Camera* worldCamera = getScene()->getCurrentCamera();
		if (!worldCamera)
		{
			return;
		}
		pass->uploadEyeMat(worldCamera->getViewMatrix());
        pass->uploadProjectionMat(worldCamera->getProjectionMatrix());
    }
}
