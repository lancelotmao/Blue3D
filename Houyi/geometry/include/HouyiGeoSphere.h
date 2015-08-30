#ifndef Houyi_HouyiGeoSphere_h
#define Houyi_HouyiGeoSphere_h

#include "HouyiGeometry.h"

namespace Houyi
{
    class GeoSphere : public Geometry
    {
    public:
        GeoSphere(float r, float dtheta, float dphi);

        virtual bool generate(int vertexMode = VertexBuffer::ENormal,
                int vertexAttribute = VertexBuffer::Attribute_Texcoord | VertexBuffer::Attribute_Normal,
                int indexFlag = Generate_Line_Index | Generate_Triangle_Index);

    private:
        float mR;
        float mDtheta;
        float mDphi;
    };
}
#endif
