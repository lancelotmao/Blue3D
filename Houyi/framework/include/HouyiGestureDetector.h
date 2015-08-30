//
//  HouyiGestureDetector.h
//  Houyi
//
//  Created by Lance Mao on 18/12/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiGestureDetector_h
#define Houyi_HouyiGestureDetector_h

#include "HouyiObject.h"
#include "HouyiMotionEvent.h"
#include "HouyiVector3.h"

namespace Houyi {
    class Gesture
    {
    public:
        enum
        {
            EStart,
            ERunning
        };
        
        Gesture() : mState(EStart){}
        
        int mState;
    };
    
    class GestureListener
    {
    public:
        virtual ~GestureListener(){}
        virtual bool onSingleTapUp(const MotionEvent& e)
        {
            return false;
        }
        
        virtual bool onScroll(const MotionEvent& e, float sx, float sy)
        {
            return false;
        }

        virtual bool onFling(const MotionEvent& e, float vx, float vy)
        {
            return false;
        }
        
        virtual bool onScale(const MotionEvent& e, float scale)
        {
            return false;
        }
        
        virtual bool onRotate(const MotionEvent& e, float radian)
        {
            return false;
        }
        
        virtual bool onPanning(const Gesture& e, float dx, float dy)
        {
            return false;
        }
        
        virtual bool onDoubleClick(const MotionEvent& e, float x, float y)
        {
            return false;
        }
    };
    
    class GestureDetector : public Object
    {
    public:
        static double CLICK_TIME_THRESHOLD;
        static double TAP_THRESHOLD;
        
        GestureDetector(GestureListener* listener);
        
        // DIY
        bool onTouch(MotionEvent e);
        
        // from platform gesture recognizer
        void handlePinch(float scale);
        void handlePan(float dx, float dy);
        
        inline bool ignoreSingleMove()
        {
            return mIgnoreSingleMoveAfterScale;
        }

        inline void setScaleFactor(float f)
        {
            mScaleFactor = f;
        }

    protected:
        float mDownX;
        float mDownY;
        double mDownTime;
        float mLastX[MotionEvent::EVENT_QUEUE_LENGTH];
        float mLastY[MotionEvent::EVENT_QUEUE_LENGTH];
        double mEventTime[MotionEvent::EVENT_QUEUE_LENGTH];
        int mEventIndex;
        
        bool mCancelClick;
        bool mCancelMultiClick;
        GestureListener* mListener;
        float mScaleDis;
        float mRotateRad;
        // second pointer for rotation detection
        Vector3 mLastV;
        // only for panning so that we don't mess up with other event
        float mLastX1;
        float mLastY1;
        bool mCancelUp;
        bool mIgnoreSingleMoveAfterScale;
        double mLastReleaseTime;
        int mTapCount;
        
    private:
        bool mMultiFingerMoved;
        float mScaleFactor;
        Gesture mRotateGesture;
        Gesture mPanningGesture;
    };
}

#endif
