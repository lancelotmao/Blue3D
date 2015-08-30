//
//  HouyiViewState.h
//  Houyi
//
//  Created by Lance Mao on 16/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiViewState_h
#define Houyi_HouyiViewState_h

#include "HouyiObject.h"

namespace Houyi
{
    class ViewState : public Object
    {
        friend class ViewStateManager;
        friend class World;
        friend class Root;
    public:
        ViewState(const string name = "ViewState");
        virtual ~ViewState();
        
        void startViewState(ViewState* state);
        void finish();
        
    protected:
        virtual void onCreate();
        virtual void onResume();
        virtual void onPause();
        virtual void onDestroy();
        
        // special call back for games
        virtual void onUpdate();
        
        // happens in render thread
        virtual void onPostCreate();
        
        virtual bool onBackPressed();

        // callback in render thread
        virtual void onCommand(int command);

    private:
        virtual void create();
        virtual void resume();
        virtual void pause();
        virtual void destroy();
        
        virtual void update();

        virtual void postCreate();
        
        // default behavior is to finish top ViewState
        virtual bool backPressed();

        bool mIsCreated;
        bool mIsPostCreated;
    };
}

#endif
