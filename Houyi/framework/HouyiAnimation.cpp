#include "HouyiAnimation.h"
#include "Platform.h"
#include "HouyiLog.h"
#include "HouyiRoot.h"
#include "HouyiMath.h"

namespace Houyi
{
    Animation::Animation() : mRoot(0), mDuration(1000), mState(EIdle), mListener(0),
    		mInterpolationType(ELinear), mStartTime(0), mFrom(0), mTo(1), mCurrent(0)
    {

    }

    Animation::~Animation()
    {

    }

    void Animation::start(Root* root, double delayMilli)
    {
        mStartTime = currentTimeMillis() + delayMilli;
        mState = EWaiting;
        if (root)
        {
            root->addAnimation(this);
            mRoot = root;
        }
    }

    void Animation::start(Root* root, double from, double to, double delayMilli)
    {
        mFrom = from;
        mCurrent = mFrom;
        mTo = to;
        start(root, delayMilli);
    }

    void Animation::stop()
    {
        // note remove should happen before notify listener
        // because listener might start animation again
        if (mRoot)
        {
            mRoot->removeAnimation(this);
        }
        
        mState = EIdle;
        mStartTime = 0;
        if (mListener)
        {
            mListener->onAnimationStop();
        }
    }

    void Animation::process(Root* root, double currentTime)
    {
        if (root)
        {
            root->requestRender();
        }
        
        double animationTime = currentTime - mStartTime;
        if (animationTime < 0)
        {
            mState = EWaiting;
            return;
        }

        if (mState == EStopping)
        {
            // note remove should happen before notify listener
            // because listener might start animation again
            root->removeAnimation(this);
            
            mState = EIdle;
            if (mListener)
            {
                mListener->onAnimationStop();
            }
            return;
        }

        double ratio = 0;
        if (animationTime > mDuration)
        {
            ratio = 1.0f;
        }
        else
        {
            ratio = interpolate(animationTime / mDuration);
        }

        mCurrent = mFrom + (mTo - mFrom) * ratio;
        onAnimation(ratio);

        if (ratio >= 1.0f)
        {
            mState = EStopping;
        }
        else
        {
            mState = ERunning;
        }
    }

    float Animation::interpolate(float x)
    {
        if (mInterpolationType == ELinear)
        {
            return x;
        }
        else if (mInterpolationType == ESquare)
        {
            return x * x;
        }
        else if (mInterpolationType == ECubic)
        {
            return x * x * x;
        }
        else if(mInterpolationType == EDecelerate)
        {
            return (1 - (1 - x) * (1 - x));
        }
        else if (mInterpolationType == ESinoid)
        {
            return (sin(x * Math::PI - Math::PI / 2) + 1) / 2;
        }
        else if (mInterpolationType == ESmoothStep)
        {
            return x*x*(3 - 2*x);
        }
        else if (mInterpolationType == EKenPerlin)
        {
            return x*x*x*(x*(x*6 - 15) + 10);
        }
        return x;
    }
}
