//
//  HouyiViewStateManager.cpp
//  Houyi
//
//  Created by Lance Mao on 16/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiViewStateManager.h"

namespace Houyi
{
    ViewStateManager* ViewStateManager::mIns = 0;
    
    ViewStateManager* ViewStateManager::getInstance()
    {
        if (!mIns)
        {
            mIns = HouyiNew ViewStateManager();
        }
        return mIns;
    }
    
    ViewStateManager::~ViewStateManager()
    {
        clear();
        mIns = 0;
    }
    
    void ViewStateManager::startViewState(ViewState* state)
    {
        if (mStack.size() > 0 && mStack.top() == state)
        {
            return;
        }
        
        if (mStack.size() > 0)
        {
            mStack.top()->pause();
        }
        
        if (!state->mIsCreated)
        {
            state->create();
        }
        
        state->resume();
        mStack.push(state);
    }
    
    void ViewStateManager::finishViewState(ViewState* state)
    {
        ViewState* vs = mStack.top();
        vs->pause();
        vs->destroy();
        HouyiDelete(vs);
        mStack.pop();
    }
    
    void ViewStateManager::finishTop()
    {
        ViewState* vs = getTop();
        if (vs)
        {
            vs->pause();
            vs->destroy();
            HouyiDelete(vs);
            mStack.pop();
        }

        // next ViewState will resume
        vs = getTop();
        if (vs)
        {
            vs->resume();
        }
    }

    void ViewStateManager::clear()
    {
        while (mStack.size() > 0) {
            ViewState* vs = mStack.top();
            HouyiDelete(vs);
            mStack.pop();
        }
    }
    
    ViewState* ViewStateManager::getTop()
    {
        return mStack.size() > 0 ? mStack.top() : 0;
    }

    void ViewStateManager::updateTop()
    {
        if (mStack.size() > 0)
        {
            ViewState* vs = mStack.top();
            vs->update();
        }
    }

    bool ViewStateManager::backPressed(ViewState* state)
    {
        return state->backPressed();
    }
}
