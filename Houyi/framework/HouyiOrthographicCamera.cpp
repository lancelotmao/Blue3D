#include "HouyiOrthographicCamera.h"

namespace Houyi
{
    OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom) :
        mLeft(left), mRight(right), mTop(top), mBottom(bottom)
    {
    }

    OrthographicCamera::~OrthographicCamera()
    {

    }

    void OrthographicCamera::peek(float xs1, float ys1, float xs2, float ys2, float speed)
    {
        // DO NOTHING
    }

    void OrthographicCamera::project()
    {
        mProjectionMatrix[0][0] = 2 / (mRight - mLeft);
        mProjectionMatrix[1][0] = 0;
        mProjectionMatrix[2][0] = 0;
        mProjectionMatrix[3][0] = 0;

        mProjectionMatrix[0][1] = 0;
        mProjectionMatrix[1][1] = 2 / (mTop - mBottom);
        mProjectionMatrix[2][1] = 0;
        mProjectionMatrix[3][1] = 0;

        mProjectionMatrix[0][2] = 0;
        mProjectionMatrix[1][2] = 0;
        mProjectionMatrix[2][2] = 0;
        mProjectionMatrix[3][2] = 0;

        mProjectionMatrix[0][3] = -(mRight + mLeft) / (mRight - mLeft);
        mProjectionMatrix[1][3] = -(mTop + mBottom) / (mTop - mBottom);
        mProjectionMatrix[2][3] = 0;
        mProjectionMatrix[3][3] = 1;
    }

    float* OrthographicCamera::getPerspectiveCompensateMatrix(float x, float y, float z)
    {
        // The matrix is
        // -z, 0, x, 0
        // 0, -z, y, 0
        // 0, 0, 1, 0
        // 0, 0, 1, -z
        mMagicMatrix[0] = mMagicMatrix[5] = mMagicMatrix[15] = -z;
        mMagicMatrix[8] = x;
        mMagicMatrix[9] = y;
        mMagicMatrix[10] = mMagicMatrix[11] = 1;

        mMagicMatrix[1] = mMagicMatrix[2] = mMagicMatrix[3] = mMagicMatrix[4] = 0;
        mMagicMatrix[6] = mMagicMatrix[7] = mMagicMatrix[12] = mMagicMatrix[13] = mMagicMatrix[14] = 0;
        return mMagicMatrix;
    }
}
