#ifndef HouyiAnimation_H
#define HouyiAnimation_H

#include "HouyiAllocator.h"

namespace Houyi
{
    class AnimationListener
    {
    public:
        virtual ~AnimationListener(){}
        virtual void onAnimationStop() = 0;
    };

    class Animation: public Allocator
    {
    public:
        enum
        {
            EIdle, EWaiting, ERunning, EStopping
        };

        enum
        {
            ELinear, ESquare, ECubic, EDecelerate, ESinoid, ESmoothStep, EKenPerlin
        };

        Animation();
        virtual ~Animation();

        virtual void start(double delayMilli = 0);
        virtual void start(double from, double to, double delayMilli = 0);
        virtual void stop();
        virtual void process(double currentTime);

        virtual void setAnimationListener(AnimationListener* listener)
        {
            mListener = listener;
        }

        virtual bool isPending()
        {
            return mState == EWaiting;
        }

        virtual bool isRunning()
        {
            return mState == ERunning;
        }
        
        virtual bool isStopping()
        {
            return mState == EStopping;
        }

        virtual bool isActive()
        {
            return mState != EIdle;
        }

        virtual void setDuration(double duration)
        {
            mDuration = duration;
        }

        virtual double getCurrent()
        {
            return mCurrent;
        }

        inline void setInterpolationType(int type)
        {
            mInterpolationType = type;
        }

    protected:
        virtual void onAnimation(double ratio)
        {
        }

        double mStartTime;
        double mDuration;
        int mState;

        double mFrom;
        double mTo;
        double mCurrent;

        AnimationListener* mListener;

    private:
        float interpolate(float x);

    private:
        int mInterpolationType;
    };
}

#endif
