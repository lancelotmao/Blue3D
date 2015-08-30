//
//  HouyiGeometry.h
//  Houyi
//
//  Created by Lance Mao on 16/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiGeometry_h
#define Houyi_HouyiGeometry_h

#include "HouyiMesh.h"

namespace Houyi
{
    class Geometry : public Mesh
    {
    public:
    	static const int Generate_Triangle_Index = 1;
    	static const int Generate_Line_Index = 1 << 1;

        virtual ~Geometry(){}
        
        Geometry(){}
        
        virtual bool generate(int vertexMode = VertexBuffer::ENormal, int vertexAttribute = VertexBuffer::Attribute_Texcoord,
        		int indexFlag = Generate_Line_Index | Generate_Triangle_Index) = 0;

        virtual bool generateEdgeLines()
        {
            return false;
        }

    };
}

#endif
