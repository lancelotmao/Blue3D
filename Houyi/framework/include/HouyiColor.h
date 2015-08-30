#ifndef __Houyi__HouyiColor__
#define __Houyi__HouyiColor__

#include "HouyiLog.h"

namespace Houyi
{
    struct Color
    {
    public:
        Color() :
                mR(0), mG(0), mB(0), mA(1)
        {
        }

        Color(float r, float g, float b, float a = 1) :
                mR(r), mG(g), mB(b), mA(a)
        {
        }
        
        Color(int c)
        {
            mA = ((c >> 24) & 0xff) / 255.0;
            mR = ((c >> 16) & 0xff) / 255.0;
            mG = ((c >> 8) & 0xff) / 255.0;
            mB = (c & 0xff) / 255.0;
        }
        
        Color operator*(const Color& color)
        {
            Color res;
            res.mR = mR * color.mR;
            res.mG = mG * color.mG;
            res.mB = mB * color.mB;
            return res;
        }

        float luminance() const
        {
            return mR * 0.212671 + mG * 0.715160 + mB * 0.072169;
        }
        
        int intValue() const
        {
            return ((int)(mA * 255) << 24) | ((int)(mR * 255) << 16) | ((int)(mG * 255) << 8) | (int)(mB * 255);
        }

        void print()
        {
            LOGD("R = %f, G = %f, B = %f, A = %f", mR, mG, mB, mA);
        }

        float mR;
        float mG;
        float mB;
        float mA;
    };
}

#endif
