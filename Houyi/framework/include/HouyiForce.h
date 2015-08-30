#ifndef Houyi_HouyiForce_h
#define Houyi_HouyiForce_h

#include "HouyiObject.h"
#include "HouyiVector3.h"

namespace Houyi
{
    class Force
    {
    public:
        Force() : mDuration (0)
        {

        }

        Vector3 mNormal;
        Vector3 mPosition;
        float mMag;
        float mOnTime;
        float mDuration;
    };
}
#endif
