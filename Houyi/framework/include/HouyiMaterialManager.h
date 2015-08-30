#ifndef __Houyi__HouyiMaterialManager__
#define __Houyi__HouyiMaterialManager__

#include <map>

#include "HouyiObject.h"
#include "HouyiMaterial.h"

namespace Houyi
{
    class MaterialManager : public Object
    {
    public:
        MaterialManager();
        ~MaterialManager();

        void deleteAll();
        Material* createMaterial(const string matId);
        
        inline void addMaterial(Material* mat)
        {
            mMaterials.insert(pair<string, Material*>(mat->getName(), mat));
        }
        
        inline int getMaterialCount()
        {
            return mMaterials.size();
        }
        
        Material* getMaterial(const string matId);
        Material* getMaterial(int index);

        static string getKeyCounter();
        
        inline Material* getLastBound()
        {
            return mLastBoundMaterial;
        }
        
        inline void setLastBound(Material* material, Pass* pass)
        {
            mLastBoundMaterial = material;
            if (mLastBoundMaterial)
            {
                mLastBoundMaterial->setPass(pass);
            }
        }
        
        inline Material& getDefMaterial()
        {
            return mDefMaterial;
        }

        inline Material& getSolid()
        {
            return mSolid;
        }
        
        inline Material& getWireFrame()
        {
            return mWireFrame;
        }

    private:
        static int KeyCounter;
        
        // materials are stored in map to avoid duplicated material
        // find by ID which is string
        map<string, Material*> mMaterials;

        // so that we can skip material upload
        Material* mLastBoundMaterial;
        
        // when render entity has no material we set to default
        Material mDefMaterial;

        // used when setting is solid
        Material mSolid;
        
        // used when setting is wireframe
        Material mWireFrame;
    };
}

#endif
