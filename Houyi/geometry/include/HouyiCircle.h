#ifndef Houyi_Circle_h
#define Houyi_Circle_h

#include "HouyiGeometry.h"

namespace Houyi
{
    class Circle : public Geometry
    {
    public:
        Circle(float r, int dtheta, bool generateNow = true);

        virtual bool generate(int vertexMode = VertexBuffer::ENormal,
                int vertexAttribute = VertexBuffer::Attribute_Texcoord | VertexBuffer::Attribute_Normal,
                int indexFlag = Generate_Line_Index | Generate_Triangle_Index);

    private:
        float mR;
        float mDTheta;
    };
}

#endif
