#include "HouyiCameraNode.h"
#include "HouyiScene.h"
#include "HouyiGLPass.h"
#include "HIShaders.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"

namespace Houyi
{
    CameraNode::CameraNode() : mAttachedScene(0)
    {
        mMaterial = HouyiNew Material();
        mMaterials.push_back(mMaterial);
        Pass* pass = GLPass::createPass(HSHADER_CAMERA);
		pass->setName("Camera Pass");
//		ShaderManager::getInstance()->addPass(pass);
		mMaterial->setPass(pass);
		mMaterial->getRenderState().disableDepthTest();
		mMaterial->setPassConfigurable(false);
    }

    CameraNode::~CameraNode()
    {
        HouyiDelete(mMaterial);
    }

    void CameraNode::attachToScene(Scene* scene)
    {
        this->mAttachedScene = scene;
        scene->addSceneNode(this);
        mAttachedScene->getWorld()->getRoot()->getRenderer()->getShaderManager()->addPass(mMaterial->getPass());

        GeoPlane* plane = HouyiNew GeoPlane(1, 1, 2, 2);
        setMesh(plane);
        scene->addMesh(plane);
        setZOrder(-999);
        scene->addRenderable(this, 0);
    }

    void CameraNode::onRenderBegin(Pass* pass)
    {
        Matrix4 m = Matrix4::IDENTITY;

        World* world = mAttachedScene->getWorld();
        Renderer* renderer = world->getRoot()->getRenderer();
        if (!renderer)
        {
        	LOGE("CameraNode::onRenderBegin. NULL renderer");
        	return;
        }

        float sw = renderer->getWidth();
        float sh = renderer->getHeight();
        float pw = mPreviewWidth;
        float ph = mPreviewHeight;

        float aspect = 1;
        if (sw > sh)
        {
            aspect = pw / ph;
        }
        else
        {
            aspect = ph / pw;
        }

        // View too wide we cannot fill full screen
        if (pw / ph < sw / sh)
        {
            float extra = sw / sh / (pw / ph);
            m[0][0] = aspect * extra;
            m[1][1] = extra;
        }
        else
        {
            m[0][0] = aspect;
        }
        m[2][3] = -1 / tan(Math::DegreesToRadians(15));

        if (!pass)
		{
        	LOGE("CameraNode::onRenderBegin. NULL pass");
        	return;
		}
        pass->uploadEyeMat(m);

//        Camera* worldCamera = Root::getInstance()->getWorld()->getCurrentCamera();
//        if (worldCamera)
//        {
//            Camera cam;
//            cam.setAspectRatio(worldCamera->getAspectRatio());
//            cam.setFOV(worldCamera->getFOV());
//            cam.setNearRange(1);
//            cam.setFarRange(100);
//            cam.perspective();
//            pass->uploadProjectionMat(cam.getProjectionMatrix());
//        }
    }

    void CameraNode::onRenderEnd(Pass* pass)
    {
//    	Camera* worldCamera = Root::getInstance()->getWorld()->getCurrentCamera();
//		if (!worldCamera)
//		{
//			return;
//		}
//		pass->uploadEyeMat(worldCamera->getViewMatrix());
//		pass->uploadProjectionMat(worldCamera->getProjectionMatrix());
    }
}
