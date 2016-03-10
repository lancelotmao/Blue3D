//
//  HouyiWorld.cpp
//  Houyi
//
//  Created by Lance Mao on 16/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiWorld.h"
#include "HouyiLog.h"
#include "HouyiRoot.h"
#include "HouyiAABB.h"
#include "HouyiRigidBody.h"
#include "HouyiUtil.h"
#include "HouyiRenderer.h"
#include "HouyiSettings.h"
#include "HouyiMeshOptimizer.h"
#include "HouyiMaterialManager.h"
#include "HouyiGrid.h"
#include "HouyiAxis.h"
#include "HouyiCameraNode.h"
#include "HouyiMap.h"
#include "HouyiView.h"
#include "HouyiFPCamera.h"

namespace Houyi
{
    World::World(const string name) : mTick(0), mRenderer(0), mFocusScene(0),
    mGestureDetector(this), mAxis(0), mCameraNode(0), mMap(0), mRootViewNode(0)
    {
        Camera* camera = HouyiNew Camera();
        mDefaulScene = HouyiNew Scene();
        mDefaulScene->addCamera(camera);
        mDefaulScene->setName("World Default Scene");
        mDefaulScene->setWorld(this);
        mScenes.push_back(mDefaulScene);

        mRootViewNode = HouyiNew View();
        mRootViewNode->setName("World Root View Node");
        mDefaulScene->getRootSceneNode()->addChild(mRootViewNode);
    }

    World::~World()
    {
        mScenes.clear();
        HouyiDelete(mMap);
        HouyiDelete(mDefaulScene);
    }

    void World::onCommand(int command)
    {
    	if(mFocusScene && mFocusScene->getCurrentCamera())
        {
        	Camera* camera = mFocusScene->getCurrentCamera();
        	if(dynamic_cast<FPCamera*>(camera))
            {
        		(static_cast<FPCamera*>(camera))->addVirtualPan();
            }
    	}
    }

    void World::create(Root* root)
    {
        ViewState::create();
        mRoot = root;
        mRenderer = mRoot->getRenderer();
    }
    
    void World::clearScene()
    {
        for (int i = mScenes.size() - 1;i >= 0;--i)
        {
            if (mScenes[i] != mDefaulScene)
            {
                mScenes[i]->pauseSkeleton();
                mScenes.erase(mScenes.begin() + i);
            }
        }
        mFocusScene = 0;
    }

    void World::postCreate()
    {
        ViewState::postCreate();
        for (int i = 0;i < mScenes.size();++i)
        {
            Scene* scene = mScenes[i];
            scene->getRootSceneNode()->postInit();
            mRoot->getRenderer()->getShaderManager()->addDefaultShader(mRoot, scene);
        }
    }

    void World::windowChanged(int width, int height)
    {
        if (mRootViewNode)
        {
            mRootViewNode->requestLayout();
        }

        onWindowChanged();
    }

    void World::onWindowChanged()
    {
    }

    void World::update()
    {
        ViewState::update();
        
        // layout function will add view to control scene if it is not added yet
        if (mRootViewNode)
        {
            mRootViewNode->layout(HRect(0, 0, mRenderer->getWidth(), mRenderer->getHeight()), this);
        }
        
        for (int i = 0;i < mScenes.size();++i)
        {
            Scene* scene = mScenes[i];
            if (scene->getRigidBodyCount() > 0 || scene->getSkeletonCount() > 0)
            {
                mRoot->requestRender();
            }

            for (int i = 0, n = scene->getRigidBodyCount();i < n; ++i)
            {
                RigidBody* rb = scene->getRigidBody(i);
                rb->update();
            }

            for (int i = 0; i < scene->getSkeletonCount(); ++i)
            {
                Skeleton* skeleton = scene->getSkeleton(i);
                skeleton->update();
            }

            if (scene->getSkeletonCount() > 0)
            {
                AABB* aabb = scene->getRootSceneNode()->getAABB();
                if (aabb)
                {
                    aabb->reset();
                }
                
                for (int i = 0; i < scene->getMeshCount(); ++i)
                {
                    Mesh* mesh = scene->getMesh(i);
                    
                    if (!mesh) continue;
                    
                    SkeletonRef& skeleton = mesh->getSkeletonRef();
                    if (!skeleton) continue;
                    
                    if (!skeleton.isHardwareEnabled())
                    {
                        mesh->updateBoneAni();
                        AABB* rootAABB = scene->getRootSceneNode()->getAABB();
                        if (rootAABB)
                        {
                            AABB* aabb = mesh->getAABB(scene);
                            aabb->reset();
                            aabb->generate(mesh);
                            rootAABB->mergeAABB(aabb);
                            if (!rootAABB->contains(aabb))
                            {
                                rootAABB->addChild(aabb);
                                aabb->setParent(rootAABB);
                            }
                        }
                    }
                }
            }
            
            // finally recursively update all node in this scene
            // this includes control scene as well
            bool more = scene->getRootSceneNode()->update(mTick);
            if (more)
            {
                requestRender();
            }
        }

        onUpdate();

        mTick++;
    }

    void World::onUpdate()
    {
    }

    void World::onClearContext()
    {
    }

    bool World::dispatchTouchEvent(MotionEvent& e)
    {
        bool res = mRootViewNode->dispatchTouchEvent(e);
        if (res)
        {
            return true;
        }

        res = mGestureDetector.onTouch(e, mRenderer);
        if (res)
        {
            return true;
        }
        
        if (mGestureDetector.ignoreSingleMove() && e.getType() == MotionEvent::TYPE_MOVE)
        {
            return true;
        }

        return onTouch(e);
    }

    bool World::onTouch(MotionEvent& e)
    {
        if (!mFocusScene || e.mTouchCount > 1)
        {
            return false;
        }

        Camera* camera = mFocusScene->getCurrentCamera();
        float sw = mRenderer->getWidth();
        float sh = mRenderer->getHeight();
        double now = currentTimeMillis();

        if (e.mType == MotionEvent::TYPE_DOWN)
        {
            mDownX = e.getX();
            mDownY = e.getY();
            mEventTime[0] = now;
        	if(!mFocusScene->useFPCamera()) {
                mLastX[0] = mDownX;
                mLastY[0] = mDownY;

                mEventIndex = 1;
                camera->enablePeekAnim();
                camera->stopPeekAnim();
                if (!camera->getAnimController().isActive())
                    camera->startPeek();
        	}
        }
        else if (e.mType == MotionEvent::TYPE_MOVE)
        {
            float newX = e.getX();
            float newY = e.getY();

            if(mFocusScene->useFPCamera()) {
            	FPCamera *fpCamera = static_cast<FPCamera*>(camera);
                float dx = newX - mDownX;
                float dy = newY - mDownY;
                float angleH = dx * Math::PI / sw / 2;
                float angleV = dy * Math::PI / sh / 2;
                fpCamera->rotate(angleH, angleV);
                mDownX = newX;
                mDownY = newY;
                return true;
            }
            float w = sw;
            float h = sh;
            float x1 = mDownX * 2 / w - 1;
            float y1 = 1 - 2 * mDownY / h;
            float x2 = newX * 2 / w - 1;
            float y2 = 1 - 2 * newY / h;
            camera->peek(x1, y1, x2, y2);

            int index = mEventIndex % MotionEvent::EVENT_QUEUE_LENGTH;
            mLastX[index] = newX;
            mLastY[index] = newY;
            mEventTime[index] = now;
            mEventIndex++;
        }
        else if (e.mType == MotionEvent::TYPE_UP)
        {
            if(!mFocusScene->useFPCamera()) {
                camera->endPeek();

                float newX = e.getX();
                float newY = e.getY();
                float w = sw;
                float h = sh;
                int index = mEventIndex < 4 ? 0 :
                        (mEventIndex + MotionEvent::EVENT_QUEUE_LENGTH - 4) % MotionEvent::EVENT_QUEUE_LENGTH;
                float x1 = mLastX[index] * 2 / w - 1;
                float y1 = 1 - 2 * mLastY[index] / h;
                float x2 = newX * 2 / w - 1;
                float y2 = 1 - 2 * newY / h;
                camera->startPeekAnim(x1, y1, x2, y2, mEventTime[index], now);
            }
        }
        return true;
    }

    bool World::onSingleTapUp(const MotionEvent& e)
    {
        LOGD("World::onSingleTapUp\n");
//        SceneNode* node = selectMesh(e.getX(), e.getY());
//        if (node)
//        {
//            node->showAABB();
//        }
        return false;
    }

    bool World::onScroll(const MotionEvent& e, float sx, float sy)
    {
        return false;
    }

    bool World::onFling(const MotionEvent& e, float vx, float vy)
    {
        LOGD("World::onFling\n");
        return false;
    }

    bool World::onBackPressed()
    {
        LOGD("World::onBackPressed\n");
        return false;
    }
    
    bool World::onScale(const MotionEvent& e, float s)
    {
    	if(mFocusScene && mFocusScene->useFPCamera())
    		return false;

        if (mFocusScene)
        {
            Camera* camera = mFocusScene->getCurrentCamera();
            camera->stopPeekAnim();
            camera->disablePeekAnim();

            // following code moves camera instead of scale the scene node
            // it has issue that objects will go out of far z
//            Vector3 forward = mCurrentCamera->getPosition() - mCurrentCamera->getLookAt();
//            mCurrentCamera->setPosition(forward / s + mCurrentCamera->getLookAt());
            
            const CameraAnimData& animData = camera->getAnimController().getTargetAnimData();
            Vector3 lookat = animData.lookAt;
            mFocusScene->getRootSceneNode()->translate(-lookat);
            mFocusScene->getRootSceneNode()->scale(s, s, s);
            mFocusScene->getRootSceneNode()->translate(lookat);
        }
        
        return true;
    }
    
    bool World::onRotate(const MotionEvent& e, float radian)
    {
        if (mFocusScene)
        {
            Camera* camera = mFocusScene->getCurrentCamera();
            camera->stopPeekAnim();
            camera->disablePeekAnim();
            const CameraAnimData& animData = camera->getAnimController().getTargetAnimData();
            Vector3 lookat = animData.lookAt;
            mFocusScene->getRootSceneNode()->translate(-lookat);
            const Vector3& f = camera->getPosition() - camera->getLookAt();
            mFocusScene->getRootSceneNode()->rotate(f.x, f.y, f.z, radian);
            mFocusScene->getRootSceneNode()->translate(lookat);
            
            if (mAxis)
            {
                mAxis->rotate(f.x, f.y, f.z, radian);
            }
        }
        return true;
    }
    
    bool World::onPanning(const Gesture& gesture, float dx, float dy)
    {
        if (mFocusScene)
        {
            Camera* camera = mFocusScene->getCurrentCamera();
            float sw = mRenderer->getWidth();
            float sh = mRenderer->getHeight();
            camera->pan(sw, sh, dx, dy);
            return true;
        }
        return false;
    }
    
    bool World::onDoubleClick(const MotionEvent& e, float x, float y)
    {
    	if(mFocusScene && mFocusScene->useFPCamera())
    		return false;

        LOGD("World::onDoubleClick\n");
        if (mFocusScene)
        {
            Camera* camera = mFocusScene->getCurrentCamera();
            if (camera && !camera->getAnimController().isPresenting())
            {
                camera->stopPeekAnim();
                camera->disablePeekAnim();
                camera->getAnimController().startTransAnim();
            }
        
            mAnimRestore.setNode(mFocusScene->getRootSceneNode());
            mAnimRestore.save();
            mAnimRestore.setIdentityAsTarget();
            mAnimRestore.setDuration(400);
            mAnimRestore.start();
            
            mAnimRestoreAxis.save();
            mAnimRestoreAxis.setIdentityAsTarget();
            mAnimRestoreAxis.setDuration(400);
            mAnimRestoreAxis.start();
            return true;
        }
        return false;
    }

    void World::requestRender()
    {
        mRoot->requestRender();
    }

    void World::clearContext()
    {
        LOGI("World::clearing context \n");
        for (int i = 0;i < SceneManager::getInstance()->getSceneCount();++i)
        {
            Scene* scene = SceneManager::getInstance()->getScene(i);
            if (scene != mFocusScene)
            {
                scene->invalidateAllTexture(); // texture is context related
            }
        }

        for (int i = 0;i < mScenes.size();++i)
        {
            Scene* scene = mScenes[i];
            scene->invalidateAllTexture(); // texture is context related
            scene->getMaterialManager()->setLastBound(0, 0);
            for (int m = 0;m < scene->getMeshCount();++m)
            {
                Mesh* mesh = scene->getMesh(m);
                VertexBufferPtr vb = mesh->getVertexBuffer();
                if (vb)
                {
                    // VBO is context relevant
                    vb->setBind(false);
                }
                for (int j = 0;j < mesh->getSurfaceCount();++j)
                {
                    Surface* surface = mesh->getSurface(j);
                    if (surface)
                    {
                        // IBO is context relevant
                        surface->unBindIndexBuffer();

                        // for surfaces that are not merged e.g. grid axis
                        VertexBufferPtr vb = surface->getVertexBuffer();
                        if (vb)
                        {
                            vb->setBind(false);
                        }
                    }
                }
            }

            for (int j = 0;j < scene->getRenderableCount();++j)
            {
                scene->getRenderable(j).mSurface->unBindIndexBuffer();
            }
        }

        onClearContext();
    }

    Camera* World::getCurrentCamera()
    {
        if (mFocusScene)
        {
            return mFocusScene->getCurrentCamera();
        }
        else
        {
            return mDefaulScene->getCurrentCamera();
        }
    }

    SceneNode* World::addAxis()
    {
        if (!mAxis)
        {
            mAxis = HouyiNew Axis();
            mAxis->attachToScene(mDefaulScene);
            mAnimRestoreAxis.setNode(mAxis);
        }
        return mAxis;
    }

    SceneNode* World::addCameraPlane(int tid)
    {
        if (!mCameraNode)
        {
            mCameraNode = HouyiNew CameraNode();
            mCameraNode->setTextureId(tid);
            mCameraNode->attachToScene(mDefaulScene);
        }
        return mCameraNode;
    }
    
    void World::pauseSkeleton()
    {
        for (int i = 0;i < mScenes.size();++i)
        {
            Scene* scene = mScenes[i];
            scene->pauseSkeleton();
        }
    }
    
    void World::resumeSkeleton()
    {
        for (int i = 0;i < mScenes.size();++i)
        {
            Scene* scene = mScenes[i];
            scene->resumeSkeleton();
        }
    }

    RenderEntity World::selectMesh(float screenX, float screenY, bool onlyAABB, bool stopWhenFirstFound)
    {
        if (!mRenderer || getSceneCount() == 0)
        {
            return RenderEntity::NullEntity;
        }
        
        float viewPortWidth = mRenderer->getWidth();
        float viewPortHeight = mRenderer->getHeight();
        
        RenderEntity res = RenderEntity::NullEntity;
        float nearest = FLT_MAX;

        for (int s = 0;s < mScenes.size();++s)
        {
            Scene* scene = mScenes[s];
            for (int i = scene->getRenderableCount() - 1;i >= 0;--i)
            {
                RenderEntity entity = scene->getRenderable(i);
                SceneNode* node = entity.mNode;

                if (!node->isSelectable() || node->getVisibility() != SceneNode::EVisible)
                {
                    continue;
                }

                AABB* aabb = node->getAABB();
                if (!aabb)
                {
                    // make sure aabb is calculated
                    aabb = node->generateAABB(scene);
                    scene->addAABBToRenderable(node);
                }

                float collisionRes = Util::collisionTest(entity, getCurrentCamera(), screenX, screenY, viewPortWidth, viewPortHeight, onlyAABB);
                if (collisionRes < nearest)
                {
                    nearest = collisionRes;
                    res = entity;
                    if (stopWhenFirstFound)
                    {
                        break;
                    }
                }
            }
        }
        return res;
    }
    
    bool World::selectNode(SceneNode* node, float screenX, float screenY, bool onlyAABB)
    {
        if (!mRenderer || !node->isSelectable() || node->getVisibility() != SceneNode::EVisible)
        {
            return false;
        }
        
        float viewPortWidth = mRenderer->getWidth();
        float viewPortHeight = mRenderer->getHeight();
        
        AABB* aabb = node->getAABB();
        if (!aabb)
        {
            // make sure aabb is calculated
            node->generateAABB(mFocusScene);
        }
        
        vector<RenderEntity> entities = node->getAllEntities();
        for (int i = entities.size() - 1;i >= 0;--i)
        {
            const RenderEntity& entity = entities[i];
            
            float collisionRes = Util::collisionTest(entity, getCurrentCamera(), screenX, screenY, viewPortWidth, viewPortHeight, onlyAABB);
            if (collisionRes != FLT_MAX)
            {
                return true;
            }
        }
        
        return false;
    }

    void World::showAABB()
    {
        if (mFocusScene)
        {
            Settings::getInstance()->setHardwareBone(false);
            mFocusScene->getRootSceneNode()->showAABB();
        }
    }
    
    void World::hideAABB()
    {
        if (mFocusScene)
        {
            mFocusScene->getRootSceneNode()->hideAABB();
        }
    }

    Map* World::getMap()
    {
        return mMap;
    }

    void World::setMap(Map* map)
    {
        mMap = map;
    }

    void World::addView(View* view)
    {
        mRootViewNode->addChild(view);
        mRootViewNode->requestLayout();
    }

    void World::sendCommand(int cmd)
    {
        mRoot->addCommand(cmd);
    }
    
    void World::setFocusScene(Scene* scene)
    {
        if (mFocusScene != scene)
        {
            mRoot->getRenderer()->getShaderManager()->addDefaultShader(mRoot, scene);
        }
        mFocusScene = scene;
        sendCommand(10000);
    }
}
