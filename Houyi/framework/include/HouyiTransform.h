#ifndef __Houyi__HouyiTransform__
#define __Houyi__HouyiTransform__

#include "HouyiMatrix4.h"

namespace Houyi
{
    class Transform {
    public:
        enum
        {
            EMatrix,
            ETranslate,
            ERotate,
            EScale
        };

        enum
        {
            EConstInterpolation,
            ELinearInterpolation,
            EBezierInterpolation
        };

        Transform(int type, float e1, float e2, float e3, float e4);
        Transform(float matrix[16]);

        Matrix4& getMatrix();
        void generateMatrix();

        int mType;
        Matrix4 mMatrix;

        // for translate use [0],[1],[2] as x y z
        // for rotate [0] is angle, in radian; [1],[2],[3] represents axis,
        // for scale [0],[1],[2] as scale x, y, z
        float mElements[4];

        int mInterpolation;
        float mInTangent[6];
        float mOutTangent[6];
    };
}

#endif
