#ifndef __Houyi__HouyiFPCamera__
#define __Houyi__HouyiFPCamera__

#include "HouyiCamera.h"
#include "HouyiView.h"
#include "HouyiMotionEvent.h"

namespace Houyi
{
    class FPCamera: public Camera, public OnTouchListener
    {
    public:
    	FPCamera(float aspectRatio = 0.5) : mDirection(0){}
        virtual ~FPCamera(){}

        virtual void prepare(Scene* scene);
        virtual void rotate(const float angleH, const float angleV);
        virtual void rotate(const float x, const float y, const float z, const float radian);
        void moveForward(const float distance,  bool currentDirection);
        void moveBack(const float distance, bool currentDirection);
        void moveLeft(const float distance);
        void moveRight(const float distance);
        void addVirtualPan();
        void update();
        virtual bool onTouch(MotionEvent event, View* v);
    protected:
        float mRadius;
        int mDirection;
        const static int FORWARD = 1 << 0;
        const static int LEFT = 1 << 1;
        const static int RIGHT = 1 << 2;
        const static int BACK = 1 << 3;
        const static int VIRTUAL_PAN_WIDTH = 20;
        const static int VIRTUAL_PAN_HEIGHT = 20;
    };
}

#endif
