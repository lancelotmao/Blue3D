//
//  HouyiRoot.h
//  Houyi
//
//  Created by Lance Mao on 18/11/12.
//
//

#ifndef Houyi_HouyiRoot_h
#define Houyi_HouyiRoot_h

#include <pthread.h>
#include <queue>
#include <deque>

#include "HouyiObject.h"
#include "HouyiMotionEvent.h"

using namespace std;

namespace Houyi
{
	class Renderer;
	class World;
	class Animation;
	class PopupWorld;

    class Root: public Allocator
    {
    public:
        enum
        {
            CMD_POST_INIT
        };
        
        enum
        {
            ERenderTypeNone,
            ERenderTypeGL,
            ERenderTypeDX
        };

        static Root* getInstance();
        ~Root();

        // set up basic stuff without valid window
        void init();

        // window is created, but width height might be 0
        void onWindowCreated();

        // called either when first time launch or screen orientation change
        void onWindowChanged(int width, int height);

        // render everything
        bool render();

        // many platform has view life cycles, we are trying to follow
        void onResume();
        void onPause();
        
        // when rendering context is lost need to reset things like VBO, Texture, etc
        void clearContext();

        // root is created when application starts. During application life-cycle
        // we might need to clear current scene without destroy root
        void clearScene();

        // handle input events
        bool onTouch(MotionEvent e);
        bool onDoubleTap(MotionEvent e, float x, float y);
        bool onBackPressed();

        Renderer* getRenderer();
        void setRenderer(Renderer* renderer);

        World* getWorld();

        // replace current world. Need synchronize with render thread
        void setWorld(World* world);

        // can happen in any thread so need lock
        void addPopupWorld(PopupWorld* world);

        inline bool hasPopupWorld()
        {
            return !mPopupWorlds.empty();
        }

        inline deque<PopupWorld*>& getPopupWorlds()
        {
            return mPopupWorlds;
        }
        
        inline int getRenderType()
        {
            return mRenderType;
        }

        int getFPS();
        void updateFPS();
        void printFPS(bool print);
        bool isRenderRequested();
        void requestRender(bool flag = true);

        void addAnimation(Animation* anim);
        void removeAnimation(Animation* anim);
        void removeAllAnimation();

        void addCommand(int cmd);

        void addDeadObject(Object* obj);
        void deleteDeadObject();

    public:
        int mVersion;
        bool mAutoFinalize;

    protected:
        Renderer* mRenderer;

        World* mWorld;

        pthread_t mGLThread;

        deque<PopupWorld*> mPopupWorlds;

    private:
        Root();
        void onCommand();
        void onAnimation();

    private:
        static Root* mIns;
        queue<int> mCommandQueue;
        pthread_mutex_t mCommandMutex;
        int mRenderType;
        bool mRenderRequested;

        int mFPS;
        int mFrameCount;
        double mStartRenderTime;
        double mElapsedTime;
        bool mPrintFPS;
        vector<Animation*> mRunnigAnims;

        // objects has to be delete on rendering thread
        // e.g. render context related stuff like VBO
        vector<Object*> mDeadObjects;
        pthread_mutex_t mDeadObjectsMutex;
    };
}

#endif
