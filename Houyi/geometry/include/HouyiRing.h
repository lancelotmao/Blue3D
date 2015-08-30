//
//  HouyiRing.h
//  Houyi
//
//  Created by Lance Mao on 20/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiRing_h
#define Houyi_HouyiRing_h

#include "HouyiGeometry.h"

namespace Houyi
{
    class Ring : public Geometry
    {
    public:
        Ring(float rInner, float rOutter, int dtheta, int start = 0, int end = 360, bool generateNow = true);
        
        virtual bool generate(int vertexMode = VertexBuffer::ENormal,
                              int vertexAttribute = VertexBuffer::Attribute_Texcoord | VertexBuffer::Attribute_Normal,
                              int indexFlag = Generate_Line_Index | Generate_Triangle_Index);
        
    private:
        float mRInner;
        float mROutter;
        float mDTheta;
        int mStart;
        int mEnd;
    };
}

#endif
