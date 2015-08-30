#ifndef Houyi_HouyiCuboid_h
#define Houyi_HouyiCuboid_h

#include "HouyiLog.h"
#include "HouyiGeometry.h"

namespace Houyi
{
    class Cuboid : public Geometry
    {
    public:

        virtual ~Cuboid(){}
        
        Cuboid(){}

        Cuboid(int xCount, int yCount, int zCount,
                float xUnit, float yUnit, float zUnit,
                bool flipV = true, bool generateNow = true) :
            mXCount(xCount), mYCount(yCount), mZCount(zCount),
            mXUnit(xUnit), mYUnit(yUnit), mZUnit(zUnit), mFlipV(flipV)
        {
        	if (generateNow)
        	{
        		generate();
        	}
        }
        
        virtual bool generate(int vertexMode = VertexBuffer::ENormal, int vertexAttribute = VertexBuffer::Attribute_Texcoord,
        		int indexFlag = Generate_Line_Index | Generate_Triangle_Index);

    private:
        int mXCount;
        int mYCount;
        int mZCount;
        float mXUnit;
        float mYUnit;
        float mZUnit;
        bool mFlipV; // Library like OpenGL has flipped v coordinate
    };
}

#endif
