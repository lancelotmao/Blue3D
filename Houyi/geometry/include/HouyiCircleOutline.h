//
//  HouyiCircleOutline.h
//  Houyi
//
//  Created by Lance Mao on 19/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiCircleOutline_h
#define Houyi_HouyiCircleOutline_h

#include "HouyiGeometry.h"

namespace Houyi
{
    class CircleOutline : public Geometry
    {
    public:
        CircleOutline(float r, int dtheta, bool generateNow = true);
        
        virtual bool generate(int vertexMode = VertexBuffer::ENormal,
                              int vertexAttribute = VertexBuffer::Attribute_Texcoord | VertexBuffer::Attribute_Normal,
                              int indexFlag = Generate_Line_Index | Generate_Triangle_Index);
        
    private:
        float mR;
        float mDTheta;
    };
}

#endif
