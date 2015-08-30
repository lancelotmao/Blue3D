#include "HouyiMaterialManager.h"

namespace Houyi
{
    int MaterialManager::KeyCounter = 0;
    
    MaterialManager::~MaterialManager()
    {
        deleteAll();
    }

    MaterialManager::MaterialManager() : mLastBoundMaterial(0)
    {
        mDefMaterial.setDiffuseColor(Color(0.5, 0.5, 0.5));
        mDefMaterial.setSpecularColor(Color(0.5, 0.5, 0.5));
        mDefMaterial.setShininess(50);
        
        mSolid.setAmbientColor(Color(1, 1, 1));
        mSolid.setDiffuseColor(Color(1, 1, 1));
        mSolid.setSpecularColor(Color(1, 1, 1));
        mSolid.setShininess(20);
        
        // rgb: 0.6823, 0.7961, 0.9608 or 174, 203, 255
        mWireFrame.setName("HouyiWireframeMat");
        mWireFrame.setAmbientColor(Color(0xFFAECBF5));
        mWireFrame.setDiffuseColor(Color(0xFFAECBF5));
        mWireFrame.setSpecularColor(Color(0xFFAECBF5));
        mWireFrame.setTextureMode(RenderState::ENoTex);
        mWireFrame.setPassConfigurable(false);
        mWireFrame.getRenderState().disableLighting();
    }

    void MaterialManager::deleteAll()
    {
        for(map<string, Material*>::iterator iterator = mMaterials.begin(); iterator != mMaterials.end(); iterator++)
        {
            Material* mat = iterator->second;
            HouyiDelete (mat);
        }
        mMaterials.clear();
    }

    Material* MaterialManager::createMaterial(const string matId)
    {
        map<string, Material*>::iterator it = mMaterials.find(matId);
        if (it != mMaterials.end())
        {
            LOGD("material already created. name = %s\n", matId.c_str());
            return it->second;
        }
        Material* mat = HouyiNew Material();
        mMaterials.insert(pair<string, Material*>(matId, mat));
        mat->setName(matId);
        return mat;
    }

    Material* MaterialManager::getMaterial(const string matId)
    {
        map<string, Material*>::iterator it = mMaterials.find(matId);
        if (it != mMaterials.end())
        {
            return it->second;
        }
        if (matId.compare("HouyiWireframeMat") == 0)
        {
            return &mWireFrame;
        }
        return NULL;
    }
    
    Material* MaterialManager::getMaterial(int index)
    {
        int size = mMaterials.size();
        if (index >= size)
        {
            return 0;
        }
        
        std::map<std::string, Material*>::iterator it = mMaterials.begin();
        for (int i = 0;i < index;++i)
        {
            it++;
        }
        return it->second;
    }
    
    string MaterialManager::getKeyCounter()
    {
        char buffer [8];
        sprintf (buffer, "%d", KeyCounter++);
        return string(buffer);
    }
}
