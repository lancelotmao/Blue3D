#include "HouyiGameWorld.h"
#include "HouyiMap.h"

namespace Houyi
{
    void GameWorld::onPostInit()
    {
        if (mMap)
        {
            mMap->onPostInit(this);
        }
    }
    
    bool GameWorld::onTouch(MotionEvent& e)
    {
        return false;
    }

    bool GameWorld::onSingleTapUp(const MotionEvent& e)
    {
        return World::onSingleTapUp(e);
    }

    bool GameWorld::onScroll(const MotionEvent& e, float sx, float sy)
    {
        if (mMap)
        {
            mMap->scroll(Vector2(sx, sy));
        }
        return false;
    }

    bool GameWorld::onFling(const MotionEvent& e, float vx, float vy)
    {
        return false;
    }

    bool GameWorld::onBackPressed()
    {
        return false;
    }

    bool GameWorld::onScale(const MotionEvent& e, float s)
    {
        return false;
    }

    bool GameWorld::onRotate(const MotionEvent& e, float radian)
    {
        return false;
    }

    bool GameWorld::onPanning(const Gesture& gesture, float dx, float dy)
    {
        return false;
    }

    bool GameWorld::onDoubleClick(const MotionEvent& e, float x, float y)
    {
        return false;
    }

    Map* GameWorld::createMap()
    {
        return HouyiNew Map();
    }
}
