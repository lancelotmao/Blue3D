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
    CubeEnvMap::CubeEnvMap(Scene* scene, const string px, const string nx, const string py,
                           const string ny, const string pz, const string nz) : mCubeTexture(0) {
        imgs[0] = ImageFactory::createImage(px);
        imgs[1] = ImageFactory::createImage(nx);
        imgs[2] = ImageFactory::createImage(py);
        imgs[3] = ImageFactory::createImage(ny);
        imgs[4] = ImageFactory::createImage(pz);
        imgs[5] = ImageFactory::createImage(nz);
        init(scene);
    }
    
    CubeEnvMap::CubeEnvMap(Scene* scene, const ImagePtr images[6]) {
        for (int i = 0;i < 6;++i) {
            imgs[i] = images[i];
        }
        init(scene);
    }
    
    void CubeEnvMap::init(Scene* scene) {
        Mesh* mesh = HouyiNew GeoSphere(100, 10, 10);
        scene->addMesh(mesh);
        setMesh(mesh);
        Material* mat = HouyiNew Material();
        mat->setPass(Root::getInstance()->getRenderer()->loadShader(HSHADER_CUBEMAP));
        mat->getRenderState().disableLighting();
        mat->getRenderState().disableDepthTest();
        mat->setPassConfigurable(false);
        addMaterial(mat);
        setZOrder(-1000);
    }
    
    CubeEnvMap::~CubeEnvMap() {
        HouyiDelete(mMaterials[0]->getPass());
        HouyiDelete(mMaterials[0]);
        HouyiDelete(mCubeTexture);
        mCubeTexture = 0;
    }

    void CubeEnvMap::onRenderBegin(Pass* pass) {
        Scene* scene = getScene();

        if (!mCubeTexture) {
            mCubeTexture = scene->getTextureManager()->createCubeMapTexture(imgs);
            scene->getTextureManager()->upload(scene->getWorld()->getRoot());
            Material* mat = getMaterial(0);
            mat->setCubeMapTexID(mCubeTexture->getTextureId());
        }

        if (!scene->getWorld()->isShowSkyBox()) {
            pass->uploadMatrix4("matEyeProjection", Matrix4::IDENTITY, true);
            return;
        }
        Camera* worldCamera = scene->getWorld()->getCurrentCamera();
        if (!worldCamera) {
            return;
        }
        
        Camera cam;
        cam.setAspectRatio(worldCamera->getAspectRatio());
        cam.setFOV(90);
        cam.perspective();
        pass->uploadMatrix4("matEyeProjection", cam.getProjectionMatrix(), true);
        
        int locCamPos = glGetUniformLocation(pass->getProgram(), "cam_pos");
        if (locCamPos >= 0) {
            Vector3 campos = worldCamera->getForward(true);
            glUniform3f(locCamPos, campos.x, campos.y, campos.z);
        }

        // view matrix will be camera matrix
        Matrix3 viewMatrix = worldCamera->getNormalMatrix();
        pass->uploadMatrix3("matEyeNormal", viewMatrix, true);
    }

    void CubeEnvMap::onRenderEnd(Pass* pass) {
    }
}
