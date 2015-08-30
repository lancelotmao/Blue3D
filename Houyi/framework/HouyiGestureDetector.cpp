//
//  HouyiGestureDetector.cpp
//  Houyi
//
//  Created by Lance Mao on 18/12/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiGestureDetector.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"

namespace Houyi
{
    double GestureDetector::CLICK_TIME_THRESHOLD = 180;
    double GestureDetector::TAP_THRESHOLD = 300;
    
    GestureDetector::GestureDetector(GestureListener* listener) :
    mListener(listener), mScaleDis(-1), mRotateRad(FLT_MAX), mCancelUp(false),
    mIgnoreSingleMoveAfterScale(false), mMultiFingerMoved(false), mScaleFactor(2),
    mLastReleaseTime(0), mTapCount(0), mCancelClick(false), mDownX(-1), mDownY(-1),
    mCancelMultiClick(false)
    {
        
    }

    bool GestureDetector::onTouch(MotionEvent e)
    {
        bool res = false;
        
        Root* root = Root::getInstance();
        Renderer* renderer = root->getRenderer();
        float screenWidth = renderer->getWidth();
        float screenHeight = renderer->getHeight();
        float clickThreshold = max(screenWidth, screenHeight) / 20;
        double now = currentTimeMillis();
        
        float x = e.getX();
        float y = e.getY();
        
        if (e.mType == MotionEvent::TYPE_DOWN)
        {
            if (mDownX == -1)
            {
                mDownX = x;
                mDownY = y;
            }
            
            if (now - mLastReleaseTime > TAP_THRESHOLD)
            {
                mTapCount = 0;
                mCancelClick = false;
            }
            
            float delX = mDownX - x;
            float delY = mDownY - y;
            if (Math::Abs(delX) > clickThreshold || Math::Abs(delY) > clickThreshold)
            {
                // don't cancel single tap when mTapCount is 0
                mCancelMultiClick = true;
                mTapCount = 0;
            }
            else
            {
                mCancelMultiClick = false;
            }
            mDownX = x;
            mDownY = y;
            mLastX[0] = mDownX;
            mLastY[0] = mDownY;
            mEventTime[0] = now;
            mEventIndex = 1;
            mCancelUp = false;
            mDownTime = now;
            if (mLastReleaseTime == 0)
            {
                mLastReleaseTime = now;
            }
            
            if (now - mLastReleaseTime < TAP_THRESHOLD)
            {
                mTapCount++;
            }
            else
            {
                mTapCount = 1;
            }
        }
        else if (e.mType == MotionEvent::TYPE_MOVE)
        {
            if (e.mTouchCount == 2 && mListener)
            {
                float x1 = e.mX[0];
                float y1 = e.mY[0];
                float x2 = e.mX[1];
                float y2 = e.mY[1];
                Vector3 v = Vector3(x2, y2, 0) - Vector3(x1, y1, 0);
                float newDis = v.length();
                if (mScaleDis < 0){
                    mScaleDis = newDis;
                }
                
                float delta = newDis - mScaleDis;
                float diagonal = sqrt(screenWidth * screenWidth + screenHeight * screenHeight);
                float normalizedDelta = delta / diagonal;
                res = mListener->onScale(e, 1 + normalizedDelta * mScaleFactor);
                mScaleDis = newDis;
                
                // detect rotation
                Vector3 vLast = mLastV;
                v.normalize();
                vLast.normalize();
                float dp = v.dotProduct(vLast);
                dp = Math::Clamp<float>(dp, -1, 1);
                float angle = acos(dp);
                Vector3 vn = Vector3(0, 0, 1);
                Vector3 v3 = vLast.crossProduct(v);
                if (vn.dotProduct(v3) < 0)
                {
                    angle = -angle;
                }
                
                if (mRotateRad == FLT_MAX)
                {
                    mRotateRad = 0;
                }
                else
                {
                    mRotateRad = angle;
                }
                if (mRotateGesture.mState == Gesture::EStart)
                {
                    mRotateGesture.mState = Gesture::ERunning;
                }
                else
                {
                    res |= mListener->onRotate(e, mRotateRad);
                }
                mLastV = Vector3(x2, y2, 0) - Vector3(x1, y1, 0);
                
                // detect two finger panning
//                LOGD("r = %.4f, d = %.4f", mRotateRad, normalizedDelta);
//                if (mRotateRad < 0.01 && normalizedDelta < 0.001)
                {
                    Vector3 d = Vector3(x1, y1, 0) - Vector3(mLastX1, mLastY1, 0);
                    d.x /= screenWidth;
                    d.y /= screenHeight;
                    
                    if (mPanningGesture.mState == Gesture::EStart)
                    {
                        mPanningGesture.mState = Gesture::ERunning;
                    }
                    else
                    {
                        res |= mListener->onPanning(mPanningGesture, d.x, d.y);
                    }
                }
                mLastX1 = x1;
                mLastY1 = y1;

                mCancelUp = true;
                mMultiFingerMoved = true;
            }
            else
            {
                if (e.mTouchCount == 1 && mMultiFingerMoved)
                {
                    mIgnoreSingleMoveAfterScale = true;
                }
                float delX = x - mDownX;
                float delY = y - mDownY;
                if (Math::Abs(delX) > clickThreshold || Math::Abs(delY) > clickThreshold)
                {
                    mCancelClick = true;
                    mCancelMultiClick = true;
                }
                
                if (mListener && now - mDownTime > CLICK_TIME_THRESHOLD)
                {
                    res |= mListener->onScroll(e, x - MotionEvent::mLastX, y - MotionEvent::mLastY);
                }
                int index = mEventIndex % MotionEvent::EVENT_QUEUE_LENGTH;
                mLastX[index] = x;
                mLastY[index] = y;
                mEventTime[index] = now;
                mEventIndex++;
            }
        }
        else if (e.mType == MotionEvent::TYPE_UP)
        {
            if (!mCancelUp && mListener)
            {
                if (!mCancelMultiClick && mTapCount == 2)
                {
                    res |= mListener->onDoubleClick(e, x, y);
                }
                else if (!mCancelClick && now - mDownTime < CLICK_TIME_THRESHOLD)
                {
                    res = mListener->onSingleTapUp(e);
                }
                else
                {
                    int index = mEventIndex < 4 ? 0 : (mEventIndex + MotionEvent::EVENT_QUEUE_LENGTH - 4) % MotionEvent::EVENT_QUEUE_LENGTH;
                    double speed = Math::Clamp<double>(100.0 / (now - mEventTime[index]), 0, 100);
                    res = mListener->onFling(e, (x - mLastX[index]) * speed, (y - mLastY[index]) * speed);
                }
            }
            mScaleDis = -1;
            mRotateRad = FLT_MAX;
            mRotateGesture.mState = Gesture::EStart;
            mPanningGesture.mState = Gesture::EStart;
            
            if (e.mTouchCount >= 2 && mMultiFingerMoved)
            {
                mIgnoreSingleMoveAfterScale = true;
            }
            else if (e.mTouchCount <= 1)
            {
                // last finger up
                mIgnoreSingleMoveAfterScale = false;
                mMultiFingerMoved = false;
            }
                
            mLastReleaseTime = now;
        }
        
        return res;
    }
    
    void GestureDetector::handlePinch(float scale)
    {
        if (mListener)
        {
            MotionEvent dummy(0, 0, 0);
            mListener->onScale(dummy, scale);
        }
    }
    
    void GestureDetector::handlePan(float dx, float dy)
    {
        if (mListener)
        {
            Gesture dummy;
            mListener->onPanning(dummy, dx, dy);
        }
    }
}
