#ifndef __Houyi__HouyiGameWorldOrtho__
#define __Houyi__HouyiGameWorldOrtho__

#include "HouyiWorld.h"

namespace Houyi
{
    class GameWorld : public World
    {
    public:
        virtual void onPostInit();
        virtual bool onTouch(MotionEvent& e);
        virtual bool onSingleTapUp(const MotionEvent& e);
        virtual bool onScroll(const MotionEvent& e, float sx, float sy);
        virtual bool onFling(const MotionEvent& e, float vx, float vy);
        virtual bool onBackPressed();
        virtual bool onScale(const MotionEvent& e, float s);
        virtual bool onRotate(const MotionEvent& e, float radian);
        virtual bool onPanning(const Gesture& gesture, float dx, float dy);
        virtual bool onDoubleClick(const MotionEvent& e, float x, float y);

        // map factory
        virtual Map* createMap();
    };
}
#endif
