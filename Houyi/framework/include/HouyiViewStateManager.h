//
//  HouyiViewStateManager.h
//  Houyi
//
//  Created by Lance Mao on 16/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiViewStateManager_h
#define Houyi_HouyiViewStateManager_h

#include <stack>
#include "HouyiViewState.h"

namespace Houyi
{
    class ViewStateManager : public Object
    {
    public:
        static ViewStateManager* getInstance();
        ViewStateManager();
        virtual ~ViewStateManager();
        
        void startViewState(ViewState* state);
        void finishViewState(ViewState* state);
        void finishTop();
        void clear();
        
        ViewState* getTop();

        // call top ViewState's update method
        void updateTop();
        
        inline int getViewStateCount()
        {
            return mStack.size();
        }

        bool backPressed(ViewState* state);

    private:
        static ViewStateManager* mIns;
        stack<ViewState*> mStack;
    };
}

#endif
