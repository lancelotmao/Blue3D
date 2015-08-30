//
//  HouyiPlane.h
//  Houyi
//
//  Created by Lance Mao on 15/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiGeoPlane_h
#define Houyi_HouyiGeoPlane_h

#include "HouyiLog.h"
#include "HouyiGeometry.h"

namespace Houyi
{
    class GeoPlane : public Geometry
    {
    public:

        static const int Attribute_FillZ = 1 << 24;

        virtual ~GeoPlane(){}
        
        GeoPlane(){}

        GeoPlane(int horizontalCount, int verticalCount, float horizontalUnit,
                float verticalUnit, bool flipV = true, bool generateNow = true) :
            mHorizontalCount(horizontalCount),
            mVerticalCount(verticalCount),
            mHorizontalUnit(horizontalUnit),
            mVerticalUnit(verticalUnit),
            mFlipV(flipV)
        {
            if (generateNow)
            {
                generate();
            }
        }
        
        virtual bool generate(int vertexMode = VertexBuffer::ENormal,
                int vertexAttribute = VertexBuffer::Attribute_Texcoord | VertexBuffer::Attribute_Normal,
                int indexFlag = Generate_Line_Index | Generate_Triangle_Index);

    private:
        int mHorizontalCount;
        int mVerticalCount;
        float mHorizontalUnit;
        float mVerticalUnit;
        bool mFlipV; // Library like OpenGL has flipped v coordinate
    };
}

#endif
