#ifndef __Houyi__HouyiSceneNode__
#define __Houyi__HouyiSceneNode__

#include "HouyiNode.h"
#include "HouyiMesh.h"
#include "HouyiRenderEntity.h"
#include "HouyiMatrix3.h"
#include "HouyiMatrix4.h"

namespace Houyi
{
    class Pass;
    class AABB;
    class Scene;

    class SceneNodeRenderListener
    {
    public:
        virtual ~SceneNodeRenderListener(){}
        virtual void onRenderBegin(Pass* pass) = 0;
        virtual void onRenderEnd(Pass* pass) = 0;
    };

    class SceneNode: public Node, public SceneNodeRenderListener
    {
    public:
        enum Visibility
        {
            EVisible = 0, EInvisible, EGone
        };

        SceneNode();
        virtual ~SceneNode();

        void setScene(Scene* scene);
        Scene* getScene();
        
        inline Mesh* getMesh()
        {
            return mMesh;
        }

        inline void setMesh(Mesh* mesh)
        {
            mMesh = mesh;
            if (!mesh)
            {
                mEntities.clear();
            }
        }

        // init may happens in any thread
        void init();
        virtual void onInit();
        
        // postInit always in render thread
        void postInit();
        virtual void onPostInit();
        
        bool update(int frame = 0);
        virtual bool onUpdate()
        {
            return false;
        }
        virtual void dirtyDisplay();
        inline bool isDisplayDirty()
        {
            return mDisplayDirty;
        }
        virtual void updateViewMatix(int frame = 0);
        virtual const Matrix4& getLocal2World();
        virtual Matrix4 getLocal2Parent();
        virtual Matrix4& getLocal();
        virtual Matrix3& getLocal2WorldNormal();
        virtual void setLocal(Matrix4 matrix, bool nonUniform = false);

        virtual void setIdentity();
        virtual void translate(Vector3 trans);
        virtual void translate(float tx, float ty, float tz);
        virtual void rotate(float x, float y, float z, float radian);
        virtual void rotate(Vector3 axis, float radian);
        virtual void rotate(Quaternion q);
        virtual void scale(float sx, float sy, float sz);
        virtual void scale(const Vector3 s);
        virtual void scale(float s);
        virtual void mulMatrix(const Matrix4& m);
        
        virtual void setVisibility(Visibility v);
        virtual Visibility getVisibility();
        virtual bool isVisible();

        // axis aligned bounding box
        virtual AABB* generateAABB(Scene* scene);
        virtual void showAABB(bool all = false);
        virtual void hideAABB(bool all = true);
        virtual AABB* getAABB();
        virtual bool isAABBVisible();
        virtual void clearAABB();

        // materials
        virtual int getMaterialCount();
        virtual void addMaterial(Material* mat);
        virtual void deleteMaterial(Material* mat);
        virtual void clearMaterial();
        virtual Material* getMaterial(int index);
        virtual void setColor(Scene* scene, Color color);

        inline void setRenderListener(SceneNodeRenderListener* listener)
        {
            mRenderListener = listener;
        }

        inline SceneNodeRenderListener* getRenderListener()
        {
            return mRenderListener;
        }

        inline bool hasNegativeScale()
        {
            return mHasNegativeScale;
        }
        
        virtual void onRenderBegin(Pass* pass) override {}
        virtual void onRenderEnd(Pass* pass) override {}
        
        inline bool isSelectable()
        {
            return mSelectable;
        }
        
        inline void setSelectable(bool selectable)
        {
            mSelectable = selectable;
        }
        
        virtual SceneNode* clone();

        inline void addEntity(RenderEntity entity)
        {
            mEntities.push_back(entity);
        }

        inline size_t getEntityCount()
        {
            return mEntities.size();
        }

        inline RenderEntity getEntity(int index)
        {
            return mEntities[index];
        }

        vector<RenderEntity> getAllEntities();
        
        inline void setZOrder(int z)
        {
            mZOrder = z;
        }
        
        inline int getZOrder()
        {
            return mZOrder;
        }
        
        void setMeshForChildren(Mesh* mesh);
        
        inline void setAABB(AABB* aabb)
        {
            mAABB = aabb;
        }
        
        inline void setKFs(KeyFrame* kfs, int cnt)
        {
            mKFs = kfs;
            mKFCount = cnt;
        }
        
        inline KeyFrame* getKFs()
        {
            return mKFs;
        }
        
        inline int getKFCount()
        {
            return mKFCount;
        }

    protected:
        // we only track which scene the root node belongs to
        // as all children will belong to the same scene
        Scene* mScene;
        
        // mesh contains geometry data
        Mesh* mMesh;

        // entities belongs to this SceneNode excluding childdren's entities
        vector<RenderEntity> mEntities;

        bool mDisplayDirty;

        Matrix4 mLocal2World;
        Matrix4 mLocal;
        Matrix3 mLocal2WorldNormal;

        Visibility mVisibility;

        bool mHasNonUniform;
        bool mHasNegativeScale;

        AABB* mAABB;

        vector<Material*> mMaterials;

        SceneNodeRenderListener* mRenderListener;

        bool mSelectable;
        
        // smaller, further away
        int mZOrder;
        
        int mKFCount;
        KeyFrame* mKFs;
        int mCurFrame;
    };
}
#endif
