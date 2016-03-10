//
//  HouyiScene.h
//  Houyi
//
//  Created by Lance Mao on 17/5/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiScene_h
#define Houyi_HouyiScene_h

#include "HouyiSceneNode.h"
#include "HouyiCamera.h"
#include "HouyiLight.h"
#include "HouyiRigidBody.h"
#include "HouyiStats.h"
#include "HouyiMaterialManager.h"
#include "HouyiTextureManager.h"

namespace Houyi
{
    class Loader;
    class World;
    
    class ViewPort
    {
    public:
        HRect mBound;
        Camera* mCamera;
    };
    
    class Scene : public Object
    {
        friend class SceneOptimizer;
        
    public:
        Scene();
        virtual ~Scene();
        
        virtual SceneNode* getRootSceneNode();
        virtual void setRootSceneNode(SceneNode* node);
        virtual void addSceneNode(SceneNode* node);
        virtual void removeSceneNode(Node* node);
        virtual bool isNodeInHierarchy(Node* node);
        
        inline void addCamera(Camera* camera)
        {
            mCameras.push_back(camera);
            if (!mCurrentCamera)
            {
                mCurrentCamera = camera;
            }
        }
        
        inline int getCameraCount()
        {
            return mCameras.size();
        }
        
        inline Camera* getCamera(int index)
        {
            return mCameras.at(index);
        }
        
        inline Camera* getCurrentCamera()
        {
            return mCurrentCamera;
        }
        
        inline void setCurrentCamera(Camera* camera)
        {
            mCurrentCamera = camera;
        }
        
        inline void setCurrentCamera(int index)
        {
            mCurrentCamera = mCameras[index];
        }
        
        inline void clearCameras()
        {
            mCameras.clear();
        }
        
        void deleteCameras();
        
        Mesh* createMesh();

        void addMesh(Mesh* mesh);
        
        // without delete
        void removeMesh(Mesh* mesh);

        // remove from vector and delete as well
        void deleteMesh(Mesh* mesh);

        inline Mesh* getMesh(int index)
        {
            return mMeshes[index];
        }
        
        inline int getMeshCount()
        {
            return mMeshes.size();
        }
        
        void clearMeshes();

        void deleteMeshes();
        
        SceneNode* findSceneNodeById(int nodeId);
        
        inline void addLight(Light* light)
        {
            if (getRootSceneNode())
            {
                getRootSceneNode()->addChild(light);
                mLights.push_back(light);
            }
        }
        
        inline int getLightCount()
        {
            return mLights.size();
        }
        
        inline Light* getLight(int index)
        {
            return mLights[index];
        }
        
        inline void clearLights()
        {
            mLights.clear();
        }
        
        inline int getRenderableCount() const
        {
            return mRenderable.size();
        }
        
        inline const RenderEntity getRenderable(int index) const
        {
            return mRenderable[index];
        }
        
        inline void removeRenderable(int index)
        {
            mRenderable.erase(mRenderable.begin() + index);
        }

        inline void addSkeleton(SkeletonPtr skeleton)
        {
            mSkeletons.push_back(skeleton);
        }
        
        inline SkeletonPtr getSkeleton(int index)
        {
            return mSkeletons[index];
        }
        
        inline int getSkeletonCount()
        {
            return mSkeletons.size();
        }
        
        inline void addRigidBody(RigidBody* rb)
        {
            mRigidBodies.push_back(rb);
        }
        
        inline RigidBody* getRigidBody(int index)
        {
            return mRigidBodies[index];
        }
        
        inline int getRigidBodyCount()
        {
            return mRigidBodies.size();
        }
        
        void addTexture(Texture* texture);
        
        inline int getTextureCount()
        {
            return mTextures.size();
        }
        
        inline Texture* getTexture(int index)
        {
            return mTextures[index];
        }
        
        void deleteTexture(Texture* texture);
        void deleteAllTexture();
        void invalidateAllTexture();
        
        inline MaterialManager* getMaterialManager()
        {
            return mMaterialMan;
        }
        
        inline TextureManager* getTextureManager()
        {
            return mTextureManager;
        }
        
        inline SceneStats& getStats()
        {
            return mStats;
        }

        virtual void addRenderableTree(SceneNode* node);
        virtual void addRenderable(SceneNode* node);
        virtual void addRenderable(SceneNode* node, int index);
        
        inline void addRenderEntity(RenderEntity entity)
        {
            mRenderable.push_back(entity);
        }
        
        virtual Camera* getCameraByName(const string& name);
        virtual Light* getLightByName(const string& name);
        virtual Mesh* getMeshByName(const string& name);
        
        virtual void addDefaultLight();
        // prepare for the scene. check camera, lights, do optimization etc
        virtual void prepare(Loader* loader);
        
        inline void setZAsUpAxis() // e.g. google sketch up uses z as up axis
        {
            mZUp = true;
        }
        inline bool isZUp()
        {
            return mZUp;
        }
        
        void merge(Scene* scene);
        
        virtual SceneNode* cloneSceneNode(SceneNode* src);
        virtual void addEntityTree(SceneNode* node);
        virtual void addAABBToRenderable(SceneNode* node);
        
        inline bool hasNegativeScale()
        {
            return mHasNegativeScale;
        }
        
        inline void setHasNegativeScale(bool has)
        {
            mHasNegativeScale = has;
        }
        
        inline void setCubeMapTexture(Texture* cubemapTexture)
        {
            mCubeMapTexture = cubemapTexture;
        }
        
        inline Texture* getCubeMapTexture()
        {
            return mCubeMapTexture;
        }
        
        inline void setUseFPCamera(bool flag)
        {
            mUseFPCamera = flag;
        }

        inline bool useFPCamera()
        {
            return mUseFPCamera;
        }

        void pauseSkeleton();
        void resumeSkeleton();
        inline bool isSkeletonPaused()
        {
            return mSkeletonPaused;
        }
        
        World* getWorld();
        void setWorld(World* world);
        
        SceneNode* addGrid();
 
        void createFourViewScene(float viewWidth, float viewHeight);
 
        // debug
        void dumpHierarchy();
        void dumpNode(SceneNode* node, int level);
        
        vector<ViewPort> mViewPorts;
        
    private:
        void deleteSceneNode(Node* node);
        bool _isNodeInHierarchy(Node* node, Node* cur);
        
    private:
        World* mWorld;
        
        // by default scene owns its data
        // when merged with other scene it passes ownership to the merged scene
        bool mOwnsData;
        SceneNode* mRootSceneNode;
        
        vector<RenderEntity> mRenderable;
        vector<Mesh*> mMeshes;
        vector<Camera*> mCameras;
        vector<Light*> mLights;
        vector<SkeletonPtr> mSkeletons;
        vector<RigidBody*> mRigidBodies;
        vector<Texture*> mTextures;
        MaterialManager* mMaterialMan;
        TextureManager* mTextureManager;
        
        Camera* mCurrentCamera;
        bool mZUp;
        SceneStats mStats;
        bool mHasNegativeScale;
        Texture* mCubeMapTexture;
        bool mSkeletonPaused;
        SceneNode* mGrid;
        bool mUseFPCamera;
    };
}

#endif
