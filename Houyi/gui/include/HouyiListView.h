//
//  HouyiListView.h
//  Pangu
//
//  Created by Lance Mao on 15/12/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef HouyiListView_h
#define HouyiListView_h

#include <deque>
#include <vector>
#include "HouyiAbsListView.h"
#include "HouyiCamera.h"
#include "HouyiAnimation.h"

namespace Houyi
{
    class AdapterView : public Allocator
    {
    public:
        AdapterView(View* view) : mView(view), mIndex(-1), mDirty(true)
        {
            
        }
        
        virtual ~AdapterView()
        {
            HouyiDelete(mView);
        }
        
        View* mView;
        int mIndex;
        bool mDirty;
    };

    class ListItemClickListener
    {
    public:
        virtual ~ListItemClickListener(){}
        virtual void onItemClicked(View* view, int index) = 0;
    };

    class ListView : public AbsListView
    {
    public:
        enum Direction
        {
            EVertical,
            EHorizontal
        };
        
        ListView(Direction dir);
        virtual ~ListView();
        
        virtual bool onLayout(const HRect& bound, World* world) override;
        virtual bool onUpdate() override;
        virtual bool onTouch(MotionEvent e) override;
        virtual bool onSingleTapUp(const MotionEvent& e) override;
        virtual bool onFling(const MotionEvent& e, float vx, float vy) override;

        virtual void scroll(float delta) override;
        virtual void setFocus(int focus);
        virtual int getFocus();

        inline void setItemClickListener(ListItemClickListener* listener)
        {
            mItemClickListener = listener;
        }
        
        virtual void onDataChanged() override;

        inline void requestCenterFocus()
        {
            mCenterFocusRequested = true;
        }
        
        virtual void setVisibility(Visibility v) override;

    protected:
        virtual AdapterView* getValidView(int index);
        virtual AdapterView* getReusableView();
        virtual float maxScroll();
        void clampScroll();
        
    protected:
        Direction mDirection;
        int mVisibleFirst;
        int mVisibleLast;
        float mScroll;
        float mViewLength; // temp solution with constant view length
        float mDownX;
        Animation* mFlingAnim;
        
        deque<AdapterView*> mReusableViewQueue;
        vector<AdapterView*> mViewContainer;

        int mFocus;
        bool mCenterFocusRequested;
        ListItemClickListener* mItemClickListener;
    };
}

#endif
