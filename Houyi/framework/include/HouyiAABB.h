#ifndef __Houyi__HouyiAABB__
#define __Houyi__HouyiAABB__

#include "HouyiSceneNode.h"
#include "HouyiSurface.h"

namespace Houyi
{
    class Scene;
    
    class AABB : public SceneNode
    {
    public:
        AABB(Scene* scene);
        virtual ~AABB();
        
        virtual void init();

        // reset Min Max vector
        virtual void reset();

        // merge with Node, not AABB because we need to get node's transform
        // for AABB transform is always identity
        virtual void merge(SceneNode* node);

        // merge directly with AABB. for example, in case of skeleton
        virtual void mergeAABB(AABB* aabb);

        virtual void updateRenderVB();
        virtual void updateCollisionVB();

        inline SurfacePtr getRenderSurface()
        {
            return mMesh ? mMesh->getSurface(0) : 0;
        }

        inline float* getAABBVertices()
        {
            return mAABBVertices;
        }

        inline VertexBufferPtr getCollisionVB()
        {
            return mCollisionVB;
        }

        inline bool isValid()
        {
            return mIsValid;
        }
        
        inline bool isAddedToWorld()
        {
            return mIsAddedToWorld;
        }
        
        inline void setAddedToWorld(bool added)
        {
            mIsAddedToWorld = added;
        }

        void generate(Mesh* mesh);
        void generate(SurfacePtr surface);
        
        virtual SceneNode* clone();

    protected:
        void calculateMinMax(Vector3& min, Vector3& max, const float* data, int length);
        void fillDataUsingMinMax(const Vector3& min, const Vector3& max);
        void mergeMinMax(Vector3& min, Vector3& max, const Vector3& min1, const Vector3& max1, const Vector3& min2, const Vector3& max2);

    protected:
        float mAABBVertices[8 * 3];
        VertexBufferPtr mCollisionVB;
        Vector3 mMin;
        Vector3 mMax;
        bool mIsValid;
        bool mIsAddedToWorld;
    };
}

#endif
