#ifndef __Houyi__HouyiMesh__
#define __Houyi__HouyiMesh__

#include "HouyiSurface.h"
#include "HouyiVertexBuffer.h"
#include "HouyiIndexBuffer.h"
#include "HouyiTexture.h"
#include "HouyiColor.h"
#include "HouyiMaterial.h"
#include "HouyiSkeleton.h"
#include "HouyiRenderState.h"

namespace Houyi
{
    class AABB;
    class EdgeLineMesh;
    class Scene;

    class Mesh: public Object
    {
    public:
        enum
        {
            ECompact,
            EExpanded
        };

    public:
        virtual ~Mesh();
        Mesh();
        virtual Mesh& operator=(const Mesh& mesh);

        inline void setCompactVertexBuffer(VertexBufferPtr vb)
        {
            mCompact = vb;
        }

        inline void setExpandedVertexBuffer(VertexBufferPtr vb)
        {
            mExpanded = vb;
        }

        inline VertexBufferPtr getVertexBuffer()
        {
            return mMode == ECompact ? mCompact : mExpanded;
        }

        inline int getMode()
        {
            return mMode;
        }

        virtual SurfacePtr getSurface(int index);
        virtual void addSurface(SurfacePtr vertexBuffer);
        virtual int getSurfaceCount();

        virtual void setTexture(Texture* texture);
        virtual Texture* getTexture();
        virtual int getTextureId();

        virtual Color& getColorRef();
        virtual void setColor(Color color);
        virtual bool isColorSet();

        virtual Material* getMaterial();
        virtual void setMaterial(Material* material);

        // function for bone animation
        virtual void updateBoneAni();// CPU update. slow
        inline void setSkeletonRef(SkeletonRef skeleton)
        {
            mSkeleton = skeleton;
        }
        inline SkeletonRef& getSkeletonRef()
        {
            return mSkeleton;
        }
        
        virtual float* getUploadBoneMatrix();
        virtual void setBindShapeMatrix(Matrix4 m);
        virtual Matrix4& getBindShapeMatrix();
        virtual bool bindToSkeleton(Skeleton* skeleton, string jointName);
        
        // shadow
        virtual void enableShadowCast(bool enable = true);
        virtual bool isShadowCastEnabled();
        virtual void setShadowFactor(float factor);
        virtual float getShadowFactor();
        virtual void setShadowSurface(SurfacePtr surface);
        virtual SurfacePtr getShadowSurface();

        void expand();
        void compact();

        AABB* getAABB(Scene* scene);
        
        void setRenderMode(int mode);

    protected:
        int mMode; // 0 compact, 1 expanded
        VertexBufferPtr mCompact;
        VertexBufferPtr mExpanded;
        vector<SurfacePtr> mSurfaces;

        Texture* mTexture;

        Color mColor;
        bool mIsColorSet;

        Material* mMaterial;

        EdgeLineMesh* mEdgeLineMesh;

        // joint nodes reference to world joint tree
        SkeletonRef mSkeleton;
        Matrix4 mBindShapeMatrix; // transform to bind shape space
        Matrix4 mBindShapeMatrixTranspose; // for NEON
        float mUploadBoneMatrix[16 * 64];
        
        // for shadows
        bool mCastShadowEnabled;
        float mShadowFactor;
        SurfacePtr mShadowSurface; // should be very small

        // only for dynamic mesh
        AABB* mAABB;
    };
}

#endif /* defined(__Houyi__HouyiMesh__) */
