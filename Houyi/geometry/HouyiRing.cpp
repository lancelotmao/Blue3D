//
//  HouyiRing.cpp
//  Houyi
//
//  Created by Lance Mao on 20/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiRing.h"

namespace Houyi
{
    Ring::Ring(float rInner, float rOutter, int dtheta, int start, int end, bool generateNow):
    mRInner(rInner), mROutter(rOutter), mDTheta(dtheta), mStart(start), mEnd(end)
    {
        if (generateNow)
        {
            generate();
        }
    }
    
    bool Ring::generate(int vertexMode, int vertexAttribute, int indexFlag)
    {
        float rInner = mRInner;
        float rOutter = mROutter;
        float dtheta = mDTheta;
        float ratio = rOutter / rInner;
        
        vector<Vector3> vCircleI;
        vector<Vector3> vCircleO;
        for (int theta = mStart;theta <= mEnd;theta+=dtheta)
        {
            float x = mRInner * cos(Math::DegreesToRadians(theta));
            float y = mRInner * sin(Math::DegreesToRadians(theta));
            vCircleI.push_back(Vector3(x, y, 0));
            vCircleO.push_back(Vector3(x, y, 0) * ratio);
        }
        
        VertexBufferPtr vb = new VertexBuffer();
        IndexBufferPtr indexBuffer;
        if (indexFlag != 0)
        {
            indexBuffer = new IndexBuffer();
        }
        float* pos = 0;
        float* normal = 0;
        float* texcoord = 0;
        ushort* triangleIndices = 0;
        uint* lineIndices = 0;
        int vcnt = 0;
        if (vertexMode == VertexBuffer::EExpandAsTriangles)
        {
            vcnt = vCircleI.size() * 2 * 3;
            pos = new float[vcnt * 3];
            
            for (int i = 0;i < vCircleI.size() - 1;i++)
            {
                Vector3& v1 = vCircleI.at(i);
                Vector3& v2 = vCircleI.at(i + 1);
                Vector3& v3 = vCircleO.at(i);
                Vector3& v4 = vCircleO.at(i + 1);
                pos[i * 18] = v1.x;
                pos[i * 18 + 1] = v1.y;
                pos[i * 18 + 2] = v1.z;
                pos[i * 18 + 3] = v2.x;
                pos[i * 18 + 4] = v2.y;
                pos[i * 18 + 5] = v2.z;
                pos[i * 18 + 6] = v3.x;
                pos[i * 18 + 7] = v3.y;
                pos[i * 18 + 8] = v3.z;
                
                pos[i * 18 + 9] = v2.x;
                pos[i * 18 + 10] = v2.y;
                pos[i * 18 + 11] = v2.z;
                pos[i * 18 + 12] = v4.x;
                pos[i * 18 + 13] = v4.y;
                pos[i * 18 + 14] = v4.z;
                pos[i * 18 + 15] = v3.x;
                pos[i * 18 + 16] = v3.y;
                pos[i * 18 + 17] = v3.z;
            }
        }
        else
        {
            vcnt = vCircleI.size() * 2;
            if ((indexFlag & Generate_Triangle_Index) != 0)
            {
                int length = (vCircleI.size() - 1) * 2 * 3;
                triangleIndices = new ushort[length];
                indexBuffer->setTriangleIndices(triangleIndices, length);
            }
            if ((indexFlag & Generate_Line_Index) != 0)
            {
                int length = (vCircleI.size() - 1) * 8;
                lineIndices = new uint[length];
                indexBuffer->setLineIndices(lineIndices, length);
            }
            pos = new float[vcnt * 3];
            if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
            {
                texcoord = new float[vcnt * 2];
            }
            float stepU = 1.0 / (vCircleI.size()-1);
            for (int i = 0;i < vCircleI.size();i++)
            {
                Vector3& v = vCircleI.at(i);
                pos[i * 6] = v.x;
                pos[i * 6 + 1] = v.y;
                pos[i * 6 + 2] = v.z;
                Vector3& v2 = vCircleO.at(i);
                pos[i * 6 + 3] = v2.x;
                pos[i * 6 + 4] = v2.y;
                pos[i * 6 + 5] = v2.z;
                
                if (texcoord)
                {
                    texcoord[i * 4] = stepU * i;
                    texcoord[i * 4 + 1] = 0;
                    texcoord[i * 4 + 2] = stepU * i;
                    texcoord[i * 4 + 3] = 1;
                }
                
                if (triangleIndices && i < vCircleI.size() - 1)
                {
                    triangleIndices[i * 6] = i * 2;
                    triangleIndices[i * 6 + 1] = (i+1) * 2;
                    triangleIndices[i * 6 + 2] = i * 2 + 1;
                    triangleIndices[i * 6 + 3] = (i+1) * 2;
                    triangleIndices[i * 6 + 4] = (i+1) * 2 + 1;
                    triangleIndices[i * 6 + 5] = i * 2 + 1;
                }
                
                if (lineIndices && i < vCircleI.size() - 1)
                {
                    lineIndices[i * 8] = i * 2;
                    lineIndices[i * 8 + 1] = i * 2 + 1;
                    lineIndices[i * 8 + 2] = i * 2 + 1;
                    lineIndices[i * 8 + 3] = (i+1) * 2 + 1;
                    lineIndices[i * 8 + 4] = (i+1) * 2 + 1;
                    lineIndices[i * 8 + 5] = (i+1) * 2;
                    lineIndices[i * 8 + 6] = (i+1) * 2;
                    lineIndices[i * 8 + 7] = i * 2;
                }
            }
        }
        
        vb->setPosition(pos, vcnt * 3);
        
        if ((vertexAttribute & VertexBuffer::Attribute_Normal) != 0)
        {
            normal = new float[vcnt * 3];
            for (int i = 0;i < vcnt;i++)
            {
                normal[i * 3] = 0;
                normal[i * 3 + 1] = 1;
                normal[i * 3 + 2] = 0;
            }
            vb->setNormal(normal, vcnt * 3);
        }
        
        if (texcoord)
        {
            vb->setTexcoord(texcoord, vcnt * 2);
        }
        
        SurfacePtr surface = new Surface(vb, indexBuffer);
        addSurface(surface);
        return true;
    }
}
