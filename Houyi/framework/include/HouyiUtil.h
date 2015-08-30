#ifndef Houyi_Util_h
#define Houyi_Util_h

#include <math.h>
#include "HouyiColor.h"
#include "HouyiLog.h"
#include "HouyiRenderEntity.h"
#include "HouyiCamera.h"

namespace Houyi
{
    static const float colors[] = {
        0.8867, 0.5937, 0.5937,
        0.8867, 0.7187, 0.5937,
        0.8906, 0.8359, 0.5937,
        0.8957, 0.8906, 0.5937,
        0.7187, 0.8906, 0.5937,
        0.5936, 0.8906, 0.5937,
        0.5937, 0.8906, 0.7187,
        0.5936, 0.8906, 0.8359,
        0.5937, 0.8398, 0.8945,
        0.5937, 0.8906, 0.8954,
        0.5937, 0.5937, 0.8954,
        0.8906, 0.5937, 0.8954,
        0.8329, 0.5937, 0.8954,
        0.8954, 0.5937, 0.8329,
        0.8906, 0.5937, 0.7187,
    };

    class Util
    {
    public:
        static Color getRandColor()
        {
            return Color((rand() % 256) / 256.0, (rand() % 256) / 256.0, (rand() % 256) / 256.0);
        }

        static int nextPowerOf2(int n);

        static int prePowerOf2(int n);

        static int roundPowerOf2(int n);

        static float ClampToZeroOne(float value);

        /**
         * Returns the approximated parameter of a parametric curve for the value X
         * @param atX At which value should the parameter be evaluated
         * @param P0_X The first interpolation point of a curve segment
         * @param C0_X The first control point of a curve segment
         * @param P1_X The second interpolation point of a curve segment
         * @param C1_X The second control point of a curve segment
         * @return The parametric argument that is used to retrieve atX using the parametric function representation of this curve
         */
        static float ApproximateCubicBezierParameter (float atX, float P0_X, float C0_X, float P1_X, float C1_X );

        static void printMatrix(float* m)
        {
            for (int i = 0; i < 4; ++i)
            {
                LOGD("M(%d) = %f, %f, %f, %f\n", i, m[i], m[4+i], m[8+i], m[12+i]);
            }
        }
        
        static float collisionTest(const RenderEntity& entity, Camera* camera, float screenX, float screenY, float viewPortWidth, float viewPortHeight, bool onlyAABB = false);

        static uint hashVertex(const VertexData& data);
        
        static void printMatrix(Matrix4 m);
    };
}

#endif
