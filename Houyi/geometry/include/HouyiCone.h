#ifndef Houyi_HouyiCone_h
#define Houyi_HouyiCone_h

#include "HouyiGeometry.h"

namespace Houyi
{
    class Cone : public Geometry
    {
    public:
        Cone(float r, float h, int dtheta);
        
        virtual bool generate(int vertexMode = VertexBuffer::ENormal,
                int vertexAttribute = VertexBuffer::Attribute_Texcoord,
                int indexFlag = Generate_Line_Index | Generate_Triangle_Index);
        
    private:
        float mR;
        float mH;
        float mDTheta;
    };
}
#endif
