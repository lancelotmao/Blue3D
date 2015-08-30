#include "HouyiCubeEnvMap.h"
#include "HouyiGeoSphere.h"
#include "HouyiImageFactory.h"
#include "HouyiMaterialManager.h"
#include "HouyiTextureManager.h"
#include "HouyiRoot.h"
#include "HouyiWorld.h"
#include "HouyiRenderer.h"
#include "HIShaders.h"

namespace Houyi
{
    CubeEnvMap::CubeEnvMap(const string px, const string py, const string pz,
                    const string nx, const string ny, const string nz)
    {
        ImagePtr imgs[6];
        imgs[0] = ImageFactory::createImage(px);
        imgs[1] = ImageFactory::createImage(pz);
        imgs[2] = ImageFactory::createImage(py);
        imgs[3] = ImageFactory::createImage(ny);
        imgs[4] = ImageFactory::createImage(nx);
        imgs[5] = ImageFactory::createImage(nz);

        mCubeTexture = TextureManager::getInstance()->createCubeMapTexture(imgs);

        Mesh* mesh = HouyiNew GeoSphere(100, 10, 10);
        Root* root = Root::getInstance();
        World* world = root->getWorld();
        world->getFocusScene()->addMesh(mesh);
        setMesh(mesh);
        Material* mat = HouyiNew Material();
        mat->setPass(ShaderManager::getInstance()->createShader(HSHADER_CUBEMAP));
        mat->getRenderState().disableLighting();
        mat->getRenderState().disableDepthTest();
        mat->addTexture(mCubeTexture);
        addMaterial(mat);
        setZOrder(-1000);
    }
    
    CubeEnvMap::~CubeEnvMap()
    {
        HouyiDelete(mMaterials[0]);
    }

    void CubeEnvMap::onRenderBegin(Pass* pass)
    {
        Camera* worldCamera = Root::getInstance()->getWorld()->getCurrentCamera();
        if (!worldCamera)
        {
            return;
        }
        Camera cam;
        cam.setAspectRatio(worldCamera->getAspectRatio());
        cam.setFOV(90);
        cam.perspective();
        pass->uploadProjectionMat(cam.getProjectionMatrix());

        Material* mat = getMaterial(0);
        mat->setCubeMapTexID(mCubeTexture->getTextureId());
    }

    void CubeEnvMap::onRenderEnd(Pass* pass)
    {
        Camera* worldCamera = Root::getInstance()->getWorld()->getCurrentCamera();
        if (!worldCamera)
        {
            return;
        }
        pass->uploadEyeMat(worldCamera->getViewMatrix());
        pass->uploadProjectionMat(worldCamera->getProjectionMatrix());
    }
}
