#ifndef __Houyi__HouyiMotionEvent__
#define __Houyi__HouyiMotionEvent__

namespace Houyi
{
    class MotionEvent
    {
    public:

        static const int MAX_TOUCH_COUNT = 10;
        
        static const int TYPE_DOWN = 0;
        static const int TYPE_MOVE = 1;
        static const int TYPE_UP = 2;
        static const int TYPE_POINTER_DOWN = 3;
        static const int TYPE_POINTER_UP = 4;

        static const int EVENT_QUEUE_LENGTH = 16;

        MotionEvent(float x, float y, int t) : mType(t)
        {
            mX[0] = x;
            mY[0] = y;
            mTouchCount = 1;
        }
        
        inline float getX(int index = 0) const 
        {
            return mX[index];
        }

        inline float getY(int index = 0) const 
        {
            return mY[index];
        }

        inline int getType()
        {
            return mType;
        }
        
        inline void setX(float x, int index = 0)
        {
            mX[index] = x;
        }
        
        inline void setY(float y, int index = 0)
        {
            mY[index] = y;
        }

        static float mLastX;
        static float mLastY;
        
        float mX[MAX_TOUCH_COUNT];
        float mY[MAX_TOUCH_COUNT];
        int mType;
        float mDownX;
        float mDownY;
        float mAbsX;
        float mAbsY;
        
        int mTouchCount;
        float mPinchDis;
    };
}

#endif
