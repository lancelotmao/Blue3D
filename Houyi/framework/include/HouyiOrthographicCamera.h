#ifndef __Houyi__HouyiOrthographicCamera__
#define __Houyi__HouyiOrthographicCamera__

#include "HouyiCamera.h"

namespace Houyi
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float top, float bottom);
        virtual ~OrthographicCamera();

        virtual void project();
        virtual void peek(float xs1, float ys1, float xs2, float ys2, float speed);

        inline void setViewPort(float left, float right, float top, float bottom)
        {
            mLeft = left;
            mRight = right;
            mTop = top;
            mBottom = bottom;
        }

        float* getPerspectiveCompensateMatrix(float x, float y, float z);

    private:
        float mLeft;
        float mRight;
        float mTop;
        float mBottom;
        float mMagicMatrix[16];
    };
}
#endif
