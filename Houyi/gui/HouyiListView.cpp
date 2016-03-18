//
//  HouyiListView.cpp
//
//  Created by Lance Mao on 15/12/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiListView.h"
#include "HouyiUtil.h"
#include "HouyiWorld.h"

namespace Houyi
{
    ListView::ListView(Direction dir) : mDirection(dir), mVisibleFirst(0), mScroll(0), mFocus(-1),
            mCenterFocusRequested(false), mItemClickListener(0)
    {
    	mFlingAnim = HouyiNew Animation();
        mFlingAnim->setDuration(1000);
        mFlingAnim->setInterpolationType(Animation::EDecelerate);
        setName("ListView");
    }
    
    ListView::~ListView()
    {
        for (int i = 0;i < mViewContainer.size();++i)
        {
            HouyiDelete(mViewContainer[i]);
        }
        HouyiDelete(mFlingAnim);
    }
    
    bool ListView::onLayout(const HRect& bound, World* world)
    {
        if (!mAdapter)
        {
            return true;
        }
        
        Scene* scene = world->getDefaultScene();
        float total = 0;
        int size = mAdapter->getSize();
        for (int i = mVisibleFirst;i < size;++i)
        {
            AdapterView* av = getValidView(i);
            if (!av || av->mDirty)
            {
                if (av == NULL)
                {
                    // try to reuse view
                    av = getReusableView();
                }
                View* v = mAdapter->getView(i, av ? av->mView : NULL);
                if (av == NULL)
                {
                    av = HouyiNew AdapterView(v);
                    mViewContainer.push_back(av);
                }
                av->mIndex = i;
                av->mDirty = false;
            }
            
            View* v = av->mView;
            if (!v)
            {
                continue;
            }
            
            // for now, we assume each view's length is the same
            mViewLength = v->getWidth();
            
            // layout
            if (mDirection == EHorizontal)
            {
                float containerW = getWidth();
                float containerOffsetX = 0;
                if (mHGravity == EHAlignCenter && mViewLength * size < containerW)
                {
                    containerOffsetX = (containerW - mViewLength * size) / 2;
                    mScroll = 0;
                }
                else if(mCenterFocusRequested)
                {
                    int focus = mAdapter->getFocusIndex();
                    float focusDesiredX = (containerW - mViewLength) / 2;
                    mScroll = mViewLength * focus - focusDesiredX;
                    clampScroll();
                }
                float offsetX = containerOffsetX + i * mViewLength;
                v->setBound(HRect(offsetX - mScroll, mBound.mTop, mViewLength, v->getHeight()), mHAlignment, mVAlignment);
                v->layout(v->getBound(), world);
                total += v->getWidth();
                
                // done visible range
                if (offsetX - mScroll >= mBound.mWidth)
                {
                    mVisibleLast = i;
                    break;
                }
            }
            else
            {
                
            }
            
            v->setVisibility(mVisibility);
        }

        mCenterFocusRequested = false;
        return false;
    }

    bool ListView::onUpdate()
    {
        bool ret = View::onUpdate();
        
        float a = mMaterial->getTransparency();
        for (int i = 0;i < mViewContainer.size();++i)
        {
            AdapterView* av = mViewContainer[i];
            if (av->mView)
            {
                
                av->mView->getMaterial(0)->setTransparency(a);
            }
        }
        
        if (mFlingAnim->isActive())
        {
            mFlingAnim->process(mWorld->getRoot(), currentTimeMillis());
            float s = mFlingAnim->getCurrent();
//            LOGI("s = %f", s);
            scroll(s);
            ret = true;
        }
        return ret;
    }

    bool ListView::onTouch(MotionEvent e)
    {
        if (!mAdapter || mAdapter->getSize() <= 0)
        {
            return false;
        }
        
        if (e.mType == MotionEvent::TYPE_DOWN)
        {
            mDownX = e.getX();
            mFlingAnim->stop();
            return true;
        }
        else if (e.mType == MotionEvent::TYPE_MOVE)
        {
            scroll(e.getX() - mDownX);
            mDownX = e.getX();
            return true;
        }
        
        return false;
    }
    
    bool ListView::onSingleTapUp(const MotionEvent& e)
    {
        if (!mAdapter || mAdapter->getSize() <= 0)
        {
            return false;
        }
        
        float x = e.getX();
        float y = e.getY();
        for (int i = 0;i < mViewContainer.size();++i)
        {
            AdapterView* av = mViewContainer[i];
            HRect bound = av->mView->getBound();
            if (x > bound.mLeft && x < bound.mLeft + bound.mWidth && y > bound.mTop && y < bound.mTop + bound.mHeight)
            {
                if (DEBUG_EVENT)
                    LOGD("ListView Item Clicked: %d", av->mIndex);
                if (mItemClickListener)
                {
                    mItemClickListener->onItemClicked(av->mView, av->mIndex);
                }
                return true;
            }
        }
        return false;
    }

    bool ListView::onFling(const MotionEvent& e, float vx, float vy)
    {
        if (!mAdapter || mAdapter->getSize() <= 0)
        {
            return false;
        }
        
        if (mDirection == EHorizontal)
        {
            mFlingAnim->start(mWorld->getRoot(), vx/4, 0);
            if (DEBUG_EVENT)
                LOGD("ListView start fling, vx = %f", vx);
        }
        else
        {

        }
        return true;
    }

    void ListView::scroll(float delta)
    {
        mScroll -= delta;
        clampScroll();
        
        // find visible first and visible last
        mVisibleFirst = (int)(mScroll / mViewLength);
        mVisibleLast = (int)((mScroll + getWidth() + mViewLength - 1) / mViewLength);
        
        // move view whose index is out of visible range to re-use queue
        for (int i = 0;i < mViewContainer.size();++i)
        {
            AdapterView* av = mViewContainer[i];
            if (av->mIndex != -1 && (av->mIndex < mVisibleFirst || av->mIndex > mVisibleLast))
            {
                av->mIndex = -1;
                av->mView->setVisibility(SceneNode::EInvisible);
                mReusableViewQueue.push_back(av);
            }
        }
        
        // need re-layout
        requestLayout();
    }
    
    void ListView::clampScroll()
    {
        if (mScroll < 0)
        {
            mScroll = 0;
        }
        else
        {
            float max = maxScroll();
            if (mScroll > max)
            {
                mScroll = max;
            }
        }
    }

    void ListView::setFocus(int focus)
    {
        if (focus == mFocus)
        {
            return;
        }

        for (int i = 0;i < mViewContainer.size();++i)
        {
            AdapterView* av = mViewContainer[i];
            if (av->mIndex == mFocus || av->mIndex == focus)
            {
                av->mDirty = true;
            }
        }

        mFocus = focus;
    }

    int ListView::getFocus()
    {
        return mFocus;
    }

    AdapterView* ListView::getValidView(int index)
    {
        for (int i = 0;i < mViewContainer.size();++i)
        {
            AdapterView* av = mViewContainer[i];
            if (av->mIndex == index)
            {
                return av;
            }
        }
        return NULL;
    }
    
    AdapterView* ListView::getReusableView()
    {
        if (mReusableViewQueue.size() == 0)
        {
            return NULL;
        }
        
        AdapterView* v = mReusableViewQueue.front();
        mReusableViewQueue.erase(mReusableViewQueue.begin());
        return v;
    }
    
    float ListView::maxScroll()
    {
        if (mAdapter)
        {
            float ret = mViewLength * mAdapter->getSize() - mBound.mWidth;
            return ret < 0 ? 0 : ret;
        }
        return 0;
    }
    
    void ListView::onDataChanged()
    {
        for (int i = 0;i < mViewContainer.size();++i)
        {
            AdapterView* av = mViewContainer[i];
            av->mDirty = true;
        }
        requestLayout();
    }
    
    void ListView::setVisibility(Visibility v)
    {
        SceneNode::setVisibility(v);
        for (int i = 0;i < mViewContainer.size();++i)
        {
            AdapterView* av = mViewContainer[i];
            if (av->mView)
            {
                av->mView->setVisibility(v);
            }
        }
    }
}
