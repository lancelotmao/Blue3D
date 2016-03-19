//
//  HouyiRoot.cpp
//  Houyi
//
//  Created by Lance Mao on 18/11/12.
//
//

#include "HouyiRoot.h"
#include "HouyiRenderer.h"
#include "HouyiPopupWorld.h"
#include "HouyiLog.h"
#include "HouyiGLESRenderer.h"
#include "HouyiTextureManager.h"
#include "HouyiMaterialManager.h"
#include "HouyiSettings.h"
#include "HouyiViewStateManager.h"
#include "HouyiAudioManager.h"

set<string> log_set;

namespace Houyi
{
    Root::Root() :
        mAutoFinalize(false), mRenderer(0), mWorld(0),
        mElapsedTime(0), mFrameCount(0), mFPS(0), mPrintFPS(false), mRenderType(ERenderTypeNone)
    {
        pthread_mutex_init(&mCommandMutex, 0);
        pthread_mutex_init(&mDeadObjectsMutex, 0);
    }

    Root::~Root()
    {
        LOGD("Start deleting Root. mAutoFinalize = %d\n", mAutoFinalize);
        

        if (mRenderer)
        {
            HouyiDelete(mRenderer);
            mRenderer = 0;
        }
        
        deleteDeadObject();

        HouyiDelete(ViewStateManager::getInstance());
        mWorld = 0;

        Settings* settings = Settings::getInstance();
        HouyiDelete(settings);

        HouyiDelete(SceneManager::getInstance());
        HouyiDelete(AudioManager::getInstance());

        log_set.clear();

        pthread_mutex_destroy(&mCommandMutex);
        pthread_mutex_destroy(&mDeadObjectsMutex);

        mRunnigAnims.clear();
        LOGD("Root is destroyed\n");
    }

    void Root::init()
    {
        mVersion = 1;
        Renderer* renderer = 0;
        if (currentPlatform() == PLATFORM_ANDROID || currentPlatform() == PLATFORM_IOS)
        {
            renderer = HouyiNew GLESRenderer();
            mRenderType = ERenderTypeGL;
        }
        setRenderer(renderer);
    }

    void Root::onWindowCreated()
    {
        LOGD("Root::onWindowCreated()\n");

        mGLThread = pthread_self();

        if (mRenderer)
        {
            mRenderer->onWindowCreated();
        }
        else
        {
            LOGW("Root::onWindowCreated() no active renderer\n");
        }
    }

    void Root::onWindowChanged(int width, int height)
    {
        LOGD("Root::onWindowChanged() (%d, %d)\n", width, height);
        if (mRenderer)
        {
            mRenderer->onWindowChanged(width, height);
        }
        else
        {
            LOGW("Root::onWindowChanged() no active renderer\n");
        }

        World* world = mWorld;
        if (world)
        {
            world->windowChanged(width, height);
        }
        else
        {
            LOGW("Root::onWindowChanged() no world\n");
        }

        for( deque<PopupWorld*>::iterator i(mPopupWorlds.begin()), end(mPopupWorlds.end()); i != end; ++i )
        {
            PopupWorld* world = *i;
            if (world->mState == PopupWorld::EIdle)
            {
                world->windowChanged(width, height);
            }
        }

        mStartRenderTime = currentTimeMillis();
    }
    
    bool Root::render()
    {
        deleteDeadObject();
        onCommand();
        onAnimation();

        bool ret = true;
        
//        ViewStateManager::getInstance()->updateTop();
        
        mWorld = getWorld();
        if (mWorld)
        {
//            mWorld->getViewStateManager()->updateTop();
            mWorld->update();
            if (mRenderer)
            {
                ret = mRenderer->renderAll(mWorld);
            }
            else
            {
                LogOnce("Root::render() no active renderer");
                ret = false;
            }
        }
        else
        {
            LogOnce("Root::render() no world");
            ret = false;
        }

        for( deque<PopupWorld*>::iterator i(mPopupWorlds.begin()), end(mPopupWorlds.end()); i != end; ++i )
        {
            PopupWorld* world = *i;
            if (world->mState == PopupWorld::EIdle)
            {
                world->onDestroy();
                mPopupWorlds.erase(i);
            }
        }

        if (!mPopupWorlds.empty())
        {
            for( deque<PopupWorld*>::iterator i(mPopupWorlds.begin()), end(mPopupWorlds.end()); i != end; ++i )
            {
                PopupWorld* world = *i;
                int backW = mRenderer->getWidth();
                int backH = mRenderer->getHeight();
                HRect& rect = world->getRect();
                mRenderer->setViewport(rect.mLeft, rect.mTop, rect.mWidth, rect.mHeight);
                
                if (world->mState == PopupWorld::EPrepare)
                {
                    world->prepare();
                }
                world->update();
                mRenderer->renderAll(world);
                mRenderer->setWidth(backW);
                mRenderer->setHeight(backH);
                mRenderer->setViewport(0, 0, backW, backH);
            }
        }

        updateFPS();
        return ret;
    }

    void Root::addCommand(int cmd)
    {
        pthread_mutex_lock (&mCommandMutex);
        mCommandQueue.push(cmd);
        pthread_mutex_unlock (&mCommandMutex);
        requestRender();
    }

    void Root::onCommand()
    {
        int cmd = -1;

        if (mCommandQueue.size() > 0)
        {
            pthread_mutex_lock (&mCommandMutex);
            cmd = mCommandQueue.front();
            mCommandQueue.pop();
            pthread_mutex_unlock (&mCommandMutex);

            ViewState* top = ViewStateManager::getInstance()->getTop();
            if (top)
            {
                top->onCommand(cmd);
            }
        }
    }

    void Root::onAnimation()
    {
        bool hasPending = false;
        double now = currentTimeMillis();
        for (int i = 0;i < mRunnigAnims.size();++i)
        {
            Animation* a = mRunnigAnims[i];
            a->process(this, now);
            if (a->isActive())
            {
                hasPending = true;
            }
        }
        if (hasPending)
        {
            requestRender();
        }
    }

    Renderer* Root::getRenderer()
    {
        return mRenderer;
    }

    void Root::setRenderer(Renderer* renderer)
    {
        mRenderer = renderer;
    }

    World* Root::getWorld()
    {
        return mWorld;
    }

    void Root::setWorld(World* world)
    {
        if (world == mWorld)
        {
            LOGD("Root setting same world\n");
            return;
        }

        if (mWorld)
        {
            ViewStateManager::getInstance()->clear();
        }

        mWorld = world;

        if (mWorld)
        {
            ViewStateManager::getInstance()->startViewState(mWorld);
        }
        else
        {
            LOGW("set world to null\n");
        }
    }
    
    void Root::addPopupWorld(PopupWorld* world)
    {
        mPopupWorlds.push_back(world);
    }

    int Root::getFPS()
    {
        return Math::Clamp<int>(mFPS, 0, 60);
    }

    void Root::onResume()
    {
        ViewState* top = ViewStateManager::getInstance()->getTop();
        if (top)
        {
            top->resume();
        }
    }

    void Root::onPause()
    {
        ViewState* top = ViewStateManager::getInstance()->getTop();
        if (top)
        {
            top->pause();
        }
    }
    
    void Root::clearContext()
    {
        World* world = mWorld;
        if (world)
        {
            world->clearContext();
        }

        Renderer* renderer = getRenderer();
        renderer->clearContext();

        ShaderManager* sm = mRenderer->getShaderManager();
        sm->invalidateAll();
    }

    void Root::clearScene()
    {
        if (mWorld)
        {
            HouyiDelete(mWorld);
            mWorld = 0;
        }

        ShaderManager* shaderMan = mRenderer->getShaderManager();
        shaderMan->deleteAll();
    }

    bool Root::onTouch(MotionEvent e)
    {
        bool ret = false;
        
        float x = e.getX();
        float y = e.getY();
        e.mAbsX = x;
        e.mAbsY = y;
        if (e.mType == MotionEvent::TYPE_DOWN)
        {
            e.mDownX = x;
            e.mDownY = y;
            e.mLastX = x;
            e.mLastY = y;
        }

        if (!mPopupWorlds.empty())
        {
            for( deque<PopupWorld*>::iterator i(mPopupWorlds.begin()), end(mPopupWorlds.end()); i != end; ++i )
            {
                //TODO
                PopupWorld* world = *i;
                ret = world->dispatchTouchEvent(e);
                if (ret)
                {
                    break;
                }
            }
        }
        else if (mWorld)
        {
            ret = mWorld->dispatchTouchEvent(e);
        }
        
        MotionEvent::mLastX = x;
        MotionEvent::mLastY = y;
        requestRender();
        return ret;
    }

    bool Root::onDoubleTap(MotionEvent e, float x, float y)
    {
        bool ret = false;

//        if (!mPopupWorlds.empty())
//        {
//            for( deque<PopupWorld*>::iterator i(mPopupWorlds.begin()), end(mPopupWorlds.end()); i != end; ++i )
//            {
//                //TODO
//                PopupWorld* world = *i;
//                ret = world->onDoubleClick(e, x, y);
//                if (ret)
//                {
//                    break;
//                }
//            }
//        }
//        else
//        {
//            ret = getWorld()->onDoubleClick(e, x, y);
//        }

        return ret;
    }

    bool Root::onBackPressed()
    {
        bool ret = false;

        if (!mPopupWorlds.empty())
        {
            for( deque<PopupWorld*>::iterator i(mPopupWorlds.begin()), end(mPopupWorlds.end()); i != end; ++i )
            {
                //TODO
                PopupWorld* world = *i;
                ret = world->onBackPressed();
                if (ret)
                {
                    break;
                }
            }
        }
        else
        {
            World* world = getWorld();
            if (world)
            {
                ret = ViewStateManager::getInstance()->backPressed(world);
            }
        }

        return ret;
    }

    void Root::updateFPS()
    {
        double now = currentTimeMillis();
        mElapsedTime += now - mStartRenderTime;
        if (mElapsedTime > 1000)
        {
            mElapsedTime -= 1000;
            mFPS = mFrameCount;
            mFrameCount = 0;
            if (mPrintFPS)
            {
                LOGD("FPS: %d\n", mFPS);
            }
        }
        mFrameCount++;
        mStartRenderTime = now;
    }

    void Root::printFPS(bool print)
    {
        mPrintFPS = print;
    }

    bool Root::isRenderRequested()
    {
        return mRenderRequested;
    }

    void Root::requestRender(bool flag)
    {
        mRenderRequested = flag;
    }

    void Root::addAnimation(Animation* anim)
    {
        bool alreadyAdded = false;
        for (int i = 0;i < mRunnigAnims.size();++i)
        {
            Animation* a = mRunnigAnims[i];
            if (a == anim)
            {
                alreadyAdded = true;
                break;
            }
        }
        if (!alreadyAdded)
        {
            mRunnigAnims.push_back(anim);
        }
        requestRender();
    }

    void Root::removeAnimation(Animation* anim)
    {
        for (int i = mRunnigAnims.size() - 1;i >= 0;--i)
        {
            Animation* a = mRunnigAnims[i];
            if (a == anim)
            {
                mRunnigAnims.erase(mRunnigAnims.begin() + i);
                break;
            }
        }
    }

    void Root::removeAllAnimation()
    {
        mRunnigAnims.clear();
    }

    void Root::addDeadObject(Object* obj)
    {
        pthread_mutex_lock (&mDeadObjectsMutex);
        mDeadObjects.push_back(obj);
        pthread_mutex_unlock (&mDeadObjectsMutex);
        requestRender();
    }

    void Root::deleteDeadObject()
    {
        pthread_mutex_lock (&mDeadObjectsMutex);
        if (mDeadObjects.size() > 0)
        {
            LOGI("Root deleting dead object on rendering thread. count = %zu\n", mDeadObjects.size());
            for (int i = 0;i < mDeadObjects.size();++i)
            {
                HouyiDelete(mDeadObjects[i]);
            }
            mDeadObjects.clear();
            LOGI("Root deleted dead objects\n");
        }
        pthread_mutex_unlock (&mDeadObjectsMutex);
     }
}
