#ifndef Houyi_HouyiRendereEntity_h
#define Houyi_HouyiRendereEntity_h

#include "HouyiSurface.h"
#include "HouyiMaterial.h"

namespace Houyi
{
    class SceneNode;
    class RenderEntity
    {
    public:
        RenderEntity(SceneNode* node, SurfacePtr surface, Material* material);

        inline bool operator == ( const RenderEntity& r2 ) const
        {
            return mNode == r2.mNode && mSurface == r2.mSurface && mMaterial == r2.mMaterial;
        }

        inline bool operator != ( const RenderEntity& r2 ) const
        {
            return !operator==(r2);
        }
        
        bool belongsToSceneNode(SceneNode* node) const;

        SceneNode* mNode;
        SurfacePtr mSurface;
        Material* mMaterial;
        
        const static RenderEntity NullEntity;
    };
}
#endif
