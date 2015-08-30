#include "HouyiTransform.h"

namespace Houyi
{
    Transform::Transform(int type, float e1, float e2, float e3, float e4) : mType(type)
    {
        mElements[0] = e1;
        mElements[1] = e2;
        mElements[2] = e3;
        mElements[3] = e4;
        generateMatrix();
    }

    Transform::Transform(float matrix[16])
    {
        mMatrix = Matrix4(matrix);
    }

    Matrix4& Transform::getMatrix()
    {
        return mMatrix;
    }

    void Transform::generateMatrix()
    {
        if (mType == ETranslate)
        {
            mMatrix.makeTrans(mElements[0], mElements[1], mElements[2]);
        }
        else if (mType == ERotate)
        {
            mMatrix = Matrix4::IDENTITY;
            mMatrix.rotate(mElements[0], mElements[1], mElements[2], mElements[3]);
        }
        else if (mType == EScale)
        {
            mMatrix = Matrix4::IDENTITY;
            mMatrix.setScale(Vector3(mElements[0], mElements[1], mElements[2]));
        }
    }
}
