//
//  HouyiViewState.cp
//  Houyi
//
//  Created by Lance Mao on 16/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiLog.h"
#include "HouyiViewState.h"
#include "HouyiViewStateManager.h"

namespace Houyi
{
    ViewState::ViewState(const string name) : mIsCreated(false), mIsPostCreated(false)
    {
        mName = name;
    }
    
    ViewState::~ViewState()
    {
        
    }

    void ViewState::startViewState(ViewState* state)
    {
        ViewStateManager::getInstance()->startViewState(state);
    }
    
    void ViewState::finish()
    {
        ViewStateManager::getInstance()->finishViewState(this);
    }

    void ViewState::onCreate()
    {
        
    }
    
    void ViewState::onResume()
    {
        
    }
    
    void ViewState::onPause()
    {
        
    }
    
    void ViewState::onDestroy()
    {
        
    }
    
    void ViewState::onUpdate()
    {
        
    }
    
    void ViewState::onPostCreate()
    {
        
    }

    bool ViewState::onBackPressed()
    {
        return false;
    }

    void ViewState::onCommand(int command)
    {

    }

    void ViewState::create()
    {
        onCreate();
        mIsCreated = true;
        LOGD("ViewState create: %s\n", mName.c_str());
    }
    
    void ViewState::resume()
    {
        onResume();
        LOGD("ViewState resume: %s\n", mName.c_str());
    }
    
    void ViewState::pause()
    {
        onPause();
        LOGD("ViewState pause: %s\n", mName.c_str());
    }
    
    void ViewState::destroy()
    {
        onDestroy();
        mIsCreated = false;
        mIsPostCreated = false;
        LOGD("ViewState destroy: %s\n", mName.c_str());
    }
    
    void ViewState::update()
    {
        if (!mIsPostCreated)
        {
            postCreate();
            mIsPostCreated = true;
        }
        onUpdate();
    }
    
    void ViewState::postCreate()
    {
        onPostCreate();
        LOGD("ViewState postCreate: %s\n", mName.c_str());
    }

    bool ViewState::backPressed()
    {
        LOGD("ViewState backPressed: %s\n", mName.c_str());
        bool ret = onBackPressed();
        if (ret)
        {
            return ret;
        }

        ViewStateManager::getInstance()->finishTop();
        return ViewStateManager::getInstance()->getViewStateCount() > 0;
    }
}
