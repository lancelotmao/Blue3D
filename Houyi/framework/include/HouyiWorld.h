//
//  HouyiWorld.h
//  Houyi
//
//  Created by Lance Mao on 16/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiWorld_h
#define Houyi_HouyiWorld_h

#include <vector>

#include "HouyiViewState.h"
#include "HouyiSkeleton.h"
#include "HouyiCamera.h"
#include "HouyiGestureDetector.h"
#include "HouyiRenderEntity.h"
#include "HouyiSceneNodeAnimation.h"
#include "HouyiSceneManager.h"

namespace Houyi
{
    class Root;
    class Renderer;
    class RigidBody;
    class Map;
    class View;
    class Axis;
    class CameraNode;

    class World: public ViewState, public GestureListener
    {
        friend class Root;
        friend class SceneOptimizer;

    public:
        World(const string name = "World");
        virtual ~World();
        virtual void update();
        virtual void create(Root* root);
        virtual void clearScene(); // except default scene
        virtual void windowChanged(int width, int height);
        virtual void requestRender();
        virtual void clearContext();
        
        Camera* getCurrentCamera();

        virtual RenderEntity selectMesh(float screenX, float screenY, bool onlyAABB = false, bool stopWhenFirstFound = false);
        virtual bool selectNode(SceneNode* node, float screenX, float screenY, bool onlyAABB = false);

        inline GestureDetector& getGestureRecognizer()
        {
            return mGestureDetector;
        }

        SceneNode* addAxis();
        SceneNode* addCameraPlane(int tid);
        
        void pauseSkeleton();
        void resumeSkeleton();

        virtual void showAABB();
        virtual void hideAABB();

        Map* getMap();
        void setMap(Map* map);

        virtual void addView(View* view);

        virtual void sendCommand(int cmd);

        inline Scene* getDefaultScene()
        {
            return mDefaulScene;
        }
        
        inline void addScene(Scene* scene)
        {
            mScenes.push_back(scene);
            scene->setWorld(this);
            if (mScenes.size() == 1) {
                setFocusScene(scene);
            }
            postCreate();
        }
        
        inline void insertScene(Scene* scene, int index)
        {
            scene->setWorld(this);
            mScenes.insert(mScenes.begin() + index, scene);
        }
        
        inline Scene* getScene(int index)
        {
            return mScenes[index];
        }
        
        inline int getSceneCount()
        {
            return mScenes.size();
        }
        
        inline Scene* getFocusScene()
        {
            return mFocusScene;
        }
        
        inline Root* getRoot()
        {
            return mRoot;
        }
        
        void setFocusScene(Scene* scene);

        inline View* getRootView()
        {
            return mRootViewNode;
        }

        virtual void onCommand(int command);
        
    protected:
        virtual void postCreate();
        virtual void onWindowChanged();
        virtual void onUpdate();
        virtual void onClearContext();
        virtual bool dispatchTouchEvent(MotionEvent& e);
        virtual bool onTouch(MotionEvent& e);
        virtual bool onSingleTapUp(const MotionEvent& e);
        virtual bool onScroll(const MotionEvent& e, float sx, float sy);
        virtual bool onFling(const MotionEvent& e, float vx, float vy);
        virtual bool onBackPressed();
        virtual bool onScale(const MotionEvent& e, float s);
        virtual bool onRotate(const MotionEvent& e, float radian);
        virtual bool onPanning(const Gesture& gesture, float dx, float dy);
        virtual bool onDoubleClick(const MotionEvent& e, float x, float y);

    protected:
        int mTick;

        // for quick access
        Root* mRoot;
        Renderer* mRenderer;

        // World doesn't own Scene
        // Instead SceneManager owns and manages Scene(s)
        vector<Scene*> mScenes;
        
        Scene* mFocusScene;
        
        // special scene that stores control node and views
        // this scene is shared with all other scenes in the world
        // Note that views are added to control scene
        Scene* mDefaulScene;
        View* mRootViewNode;

        // touch event
        GestureDetector mGestureDetector;
        float mDownX;
        float mDownY;
        float mLastX[MotionEvent::EVENT_QUEUE_LENGTH];
        float mLastY[MotionEvent::EVENT_QUEUE_LENGTH];
        double mEventTime[MotionEvent::EVENT_QUEUE_LENGTH];
        int mEventIndex;

        Axis* mAxis;
        CameraNode* mCameraNode;
        
        SceneNodeAnimation mAnimRestore;
        SceneNodeAnimation mAnimRestoreAxis;

        Map* mMap;
    };
}

#endif
