//
//  HouyiScene.cpp
//  Houyi
//
//  Created by Lance Mao on 17/5/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiScene.h"
#include "HouyiSceneManager.h"
#include "HouyiModelViewCamera.h"
#include "HouyiLoader.h"
#include "HouyiMaterialManager.h"
#include "HouyiAABB.h"
#include "HouyiSettings.h"
#include "HouyiSceneOptimizer.h"
#include "HouyiTextureManager.h"
#include "HouyiGrid.h"
#include "HouyiFPCamera.h"

namespace Houyi
{
    Scene::Scene() : mOwnsData(true), mRootSceneNode(0),mCurrentCamera(0),
    mZUp(false), mHasNegativeScale(false), mCubeMapTexture(0), mSkeletonPaused(false),
    mGrid(0), mUseFPCamera(false)
    {
        mMaterialMan = HouyiNew MaterialManager();
    }
    
    Scene::~Scene()
    {
        if (!mOwnsData)
        {
            return;
        }
        
        deleteMeshes();
        clearLights();
        deleteCameras();
        mSkeletons.clear();
        
        for (int i = 0, n = mRigidBodies.size();i < n; ++i)
        {
            RigidBody* rb = mRigidBodies[i];
            HouyiDelete(rb);
        }
        mRigidBodies.clear();
        
        HouyiDelete(mMaterialMan);
        deleteAllTexture();
        
        deleteSceneNode(mRootSceneNode);
    }
    
    SceneNode* Scene::getRootSceneNode()
    {
    	if (!mRootSceneNode)
		{
			mRootSceneNode = SceneManager::getInstance()->createRootSceneNode();
            mRootSceneNode->setScene(this);
		}
        return mRootSceneNode;
    }
    
    void Scene::setRootSceneNode(SceneNode* node)
    {
        mRootSceneNode = node;
        node->setScene(this);
    }
    
    void Scene::addSceneNode(SceneNode* node)
    {
        getRootSceneNode()->addChild(node);
    }
    
    void Scene::removeSceneNode(Node* node)
    {
        if (node)
        {
            Node* p = node->getParent();
            if (p)
            {
                p->removeChild(node);
            }
            deleteSceneNode(node);
        }
    }
    
    bool Scene::isNodeInHierarchy(Node* node)
    {
        return _isNodeInHierarchy(getRootSceneNode(), node);
    }
    
    bool Scene::_isNodeInHierarchy(Node* node, Node* cur)
    {
        if (node == cur)
        {
            return true;
        }
        
        for (int i = 0, n = cur->childrenCount(); i < n; ++i)
        {
            if (_isNodeInHierarchy(node, cur->getChildAt(i)))
            {
                return true;
            }
        }
        
        return false;
    }
    
    Mesh* Scene::createMesh()
    {
        Mesh* mesh = HouyiNew Mesh();
        mMeshes.push_back(mesh);
        return mesh;
    }

    void Scene::addMesh(Mesh* mesh)
    {
        for (int i = 0, n = mMeshes.size();i < n;++i)
        {
            if (mMeshes[i] == mesh)
            {
                return;
            }
        }
        mMeshes.push_back(mesh);
    }

    void Scene::removeMesh(Mesh* mesh)
    {
        if (mesh)
        {
            for (int i = 0, n = mMeshes.size();i < n;++i)
            {
                if (mMeshes[i] == mesh)
                {
                    mMeshes.erase(mMeshes.begin() + i);
                    break;
                }
            }
        }
    }

    void Scene::deleteMesh(Mesh* mesh)
    {
        removeMesh(mesh);
        HouyiDelete(mesh);
    }
    
    void Scene::deleteMeshes()
    {
        for (int i = 0, n = mMeshes.size(); i < n; ++i)
        {
            Mesh* mesh = mMeshes[i];
            HouyiDelete (mesh);
        }
        mMeshes.clear();
    }
    
    SceneNode* Scene::findSceneNodeById(int nodeId)
    {
        return static_cast<SceneNode*>(getRootSceneNode()->findNodeById(nodeId));
    }

    void Scene::clearMeshes()
    {
        mMeshes.clear();
    }

    void Scene::deleteCameras()
    {
        for (int i = 0, n = mCameras.size(); i < n; ++i)
        {
            Camera* camera = mCameras[i];
            HouyiDelete (camera);
        }
        mCameras.clear();
    }
    
    void Scene::addRenderableTree(SceneNode* node)
    {
        for (int i = 0;i < node->childrenCount();++i)
        {
            SceneNode* child = static_cast<SceneNode*>(node->getChildAt(i));
            addRenderableTree(child);
        }
        addRenderable(node);
    }
   
    void Scene::addRenderable(SceneNode* node)
    {
        bool added = false;
        int zorder = node->getZOrder();
        for (int i = 0;i < mRenderable.size();++i)
        {
            if (zorder < mRenderable[i].mNode->getZOrder())
            {
                addRenderable(node, i);
                added = true;
                break;
            }
        }
        
        if (!added)
        {
            addRenderable(node, mRenderable.size());
        }
    }
    
    void Scene::addRenderable(SceneNode* node, int index)
    {
        if (!node)
        {
            LOGE("World::addRenderable NULL node\n");
            return;
        }
        Mesh* mesh = node->getMesh();
        if (mesh && mesh->getSurfaceCount() > 0)
        {
            for (int i = 0;i < mesh->getSurfaceCount();++i)
            {
                SurfacePtr surface = mesh->getSurface(i);
                if (surface)
                {
                    Material* material = node->getMaterialCount() > i ? node->getMaterial(i) : 0;
                    if (!material)
                    {
                        material = surface->getMaterial();
                        if (!material)
                        {
                            material = mesh->getMaterial();
                        }
                    }
                    
                    if (!material)
                    {
                        material = &mMaterialMan->getDefMaterial();
                        LOGD("add entity, no material, use default node id = %d, name = %s\n",
                             node->getId(), node->getName().c_str());
                    }
                    
                    RenderEntity entity(node, surface, material);
                    mRenderable.insert(mRenderable.begin() + index, entity);
                    
                    // SceneNode needs to track entity
                    node->addEntity(entity);
                }
            }
        }
        else
        {
            LOGE("World::addRenderable no mesh. node = %s\n", node->getName().c_str());
        }
    }
    
    Camera* Scene::getCameraByName(const string& name)
    {
        for (int i = 0, n = getCameraCount(); i < n; i++)
        {
            Camera* camera = getCamera(i);
            if (camera->getName().compare(name) == 0)
            {
                return camera;
            }
        }
        
        return NULL;
    }
    
    Light* Scene::getLightByName(const string& name)
    {
        for (int i = 0, n = getLightCount(); i < n; i++)
        {
            Light* light = getLight(i);
            if (light->getName().compare(name) == 0)
            {
                return light;
            }
        }
        
        return NULL;
    }
    
    Mesh* Scene::getMeshByName(const string& name)
    {
        for (int i = 0, n = getMeshCount(); i < n; i++)
        {
            Mesh* mesh = getMesh(i);
            if (mesh->getName().compare(name) == 0)
            {
                return mesh;
            }
        }
        
        return NULL;
    }
    
    void Scene::addDefaultLight()
    {
        // add eye space light whose position appears fixed
        Light* light = HouyiNew Light();
        light->setPosition(0, 0, 1);
        light->setPosFixed(true);
        getRootSceneNode()->addChild(light);
        mLights.push_back(light);
        LOGD("World has no light, add default fixed light\n");
    }
    
    void Scene::prepare(Loader* loader)
    {
        if (mCameras.size() > 0 && Settings::getInstance()->useCameraFromFile())
        {
            if (!mCurrentCamera)
            {
                mCurrentCamera = mCameras.at(0);
            }
        }
        else
        {
            deleteCameras();
        	if(loader->useFPCamera())
            {
        		mCurrentCamera = HouyiNew FPCamera();
        	}
            else
            {
        		mCurrentCamera = HouyiNew ModelViewCamera();
        	}
    		mCurrentCamera->prepare(this);

            if (mZUp)
            {
            	mCurrentCamera->rotate(1, 0, 0, 3.1415926 / 2);
            	mCurrentCamera->setUp(0, 0, 1);
            }

            mCurrentCamera->getAnimController().saveForAnim();
            addCamera(mCurrentCamera);
        }
        
        if (loader->mergeMesh())
        {
            SceneOptimizer::optimize(this, loader);
        }
        else
        {
            SceneOptimizer::prepareSceneMesh(this, loader);
        }
        
        if (getLightCount() == 0)
        {
            addDefaultLight();
        }
    }
    
    void Scene::merge(Scene* scene)
    {
        for (int i = 0;i < scene->getCameraCount();++i)
        {
            addCamera(scene->getCamera(i));
        }
        
        addSceneNode(scene->getRootSceneNode());
        
        for (int i = 0;i < scene->getRenderableCount();++i)
        {
            mRenderable.push_back(scene->getRenderable(i));
        }
        
        scene->mOwnsData = false;
        HouyiDelete(scene);
    }
    
    SceneNode* Scene::cloneSceneNode(SceneNode* src)
    {
        SceneNode* node = src->clone();
        addEntityTree(node);
        return node;
    }
    
    void Scene::addEntityTree(SceneNode* node)
    {
        for (int i = 0;i < node->getEntityCount();++i)
        {
            RenderEntity entity = node->getEntity(i);
            mRenderable.push_back(entity);
        }
        for (int i = 0;i < node->childrenCount();++i)
        {
            SceneNode* child = (SceneNode*)(node->getChildAt(i));
            addEntityTree(child);
        }
    }
    
    void Scene::addAABBToRenderable(SceneNode* node)
    {
        AABB* aabb = node->getAABB();
        if (aabb && !aabb->isAddedToWorld())
        {
            RenderEntity entity(aabb, aabb->getRenderSurface(), &getMaterialManager()->getWireFrame());
            mRenderable.push_back(entity);
            aabb->setAddedToWorld(true);
            aabb->addEntity(entity);
        }
        
        for (int i = 0;i < node->childrenCount();++i)
        {
            addAABBToRenderable((SceneNode*)(node->getChildAt(i)));
        }
    }

    void Scene::deleteSceneNode(Node* node)
    {
        if (node)
        {
            for (int i = 0, n = node->childrenCount(); i < n; ++i)
            {
                deleteSceneNode(node->getChildAt(i));
            }
            for (int i = mRenderable.size() - 1;i >= 0; --i)
            {
                RenderEntity entity = mRenderable[i];
                if (entity.mNode == node)
                {
                    mRenderable.erase(mRenderable.begin() + i);
                }
            }

            HouyiDelete(node);
        }
        else
        {
            LOGW("World::deleteSceneNode null node\n");
        }
    }
    
    void Scene::addTexture(Texture* texture)
    {
        for (int i = 0;i < mTextures.size();++i)
        {
            if (mTextures[i] == texture)
            {
                return;
            }
        }
        mTextures.push_back(texture);
    }
    
    void Scene::deleteTexture(Texture* texture)
    {
        if (!texture) return;
        
        // remove from uploading queue first
        TextureManager* tm = TextureManager::getInstance();
        tm->removeTexture(this, texture);
        
        for (int i = 0, n = mTextures.size();i < n;++i)
        {
            Texture* t = mTextures[i];
            if (t == texture)
            {
                HouyiDelete(texture);
                mTextures.erase(mTextures.begin() + i);
                break;
            }
        }
    }
    
    void Scene::deleteAllTexture()
    {
        for (int i = 0, n = mTextures.size();i < n;++i)
        {
            Texture* t = mTextures[i];
            if (t)
            {
                TextureManager* tm = TextureManager::getInstance();
                tm->removeTexture(this, t);
                HouyiDelete(t);
            }
        }
        mTextures.clear();
    }
    
    void Scene::invalidateAllTexture()
    {
        for (int i = 0, n = mTextures.size();i < n;++i)
        {
            Texture* t = mTextures[i];
            if (t)
            {
                t->setTextureId(-1);
                t->invalidate();
            }
        }
    }
    
    void Scene::pauseSkeleton()
    {
        mSkeletonPaused = true;
        for (int i = 0; i < getSkeletonCount(); ++i)
        {
            Skeleton* skeleton = getSkeleton(i);
            skeleton->pause();
        }
    }
    
    void Scene::resumeSkeleton()
    {
        for (int i = 0; i < getSkeletonCount(); ++i)
        {
            Skeleton* skeleton = getSkeleton(i);
            skeleton->resume();
        }
        mSkeletonPaused = false;
    }
    
    void Scene::dumpHierarchy()
    {
        dumpNode(getRootSceneNode(), 0);
    }
    
    void Scene::dumpNode(SceneNode* nPtr, int level)
    {
        string tab = "";
        for (int i = 0; i < level; ++i)
        {
            tab += "    ";
        }
        LOGD("%s%s\n", tab.c_str(), nPtr->getName().c_str());
        const Matrix4& m = nPtr->getLocal();
        for (int i = 0; i < 4; ++i)
        {
            LOGD("%s%.4f, %.4f, %.4f, %.4f\n", tab.c_str(), m[i][0], m[i][1], m[i][2], m[i][3]);
        }
        
        for (int i = 0; i < nPtr->childrenCount(); ++i)
        {
            dumpNode((SceneNode*)nPtr->getChildAt(i), level + 1);
        }
    }
    
    SceneNode* Scene::addGrid()
    {
        if (!mGrid)
        {
            Grid* grid = HouyiNew Grid();
            grid->attachToScene(this);
            mGrid = grid;
        }
        return mGrid;
    }
    
    void Scene::createFourViewScene(float viewWidth, float viewHeight)
    {
        Camera* camera = getCurrentCamera();
        if (camera)
        {
            float viewWidthH = viewWidth / 2;
            float viewHeightH = viewHeight / 2;
            Vector3 lookat = camera->getLookAt();
            float dis = (camera->getPosition() - lookat).length();
            
            // add front view
            Vector3 pos = lookat + Vector3(0, 0, dis);
            Camera* fvc = HouyiNew Camera(pos, lookat, 0, camera->getAspectRatio());
            fvc->setFarRange(camera->getFarRange());
            fvc->setNearRange(camera->getNearRange());
            fvc->setFOV(camera->getFOV());
            addCamera(fvc);
            ViewPort vp;
            vp.mCamera = fvc;
            vp.mBound = HRect(0, viewHeightH, viewWidthH, viewHeightH);
            mViewPorts.push_back(vp);
            
            // add top view
            pos = lookat + Vector3(0, dis, 0);
            Camera* tvc = HouyiNew Camera(pos, lookat, 0, camera->getAspectRatio());
            tvc->setFarRange(camera->getFarRange());
            tvc->setNearRange(camera->getNearRange());
            tvc->setFOV(camera->getFOV());
            addCamera(tvc);
            ViewPort tvp;
            tvp.mCamera = tvc;
            tvp.mBound = HRect(0, 0, viewWidthH, viewHeightH);
            mViewPorts.push_back(tvp);
            
            // add right view
            pos = lookat + Vector3(dis, 0, 0);
            Camera* rvc = HouyiNew Camera(pos, lookat, 0, camera->getAspectRatio());
            rvc->setFarRange(camera->getFarRange());
            rvc->setNearRange(camera->getNearRange());
            rvc->setFOV(camera->getFOV());
            addCamera(rvc);
            ViewPort rvp;
            rvp.mCamera = rvc;
            rvp.mBound = HRect(viewWidthH, 0, viewWidthH, viewHeightH);
            mViewPorts.push_back(rvp);
            
            // add perspective view
            ViewPort pvp;
            pvp.mCamera = camera;
            pvp.mBound = HRect(viewWidthH, viewHeightH, viewWidthH, viewHeightH);
            mViewPorts.push_back(pvp);
        }
    }
}
