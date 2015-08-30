#include "HouyiCuboid.h"
#include "HouyiGeoPlane.h"

namespace Houyi
{
    bool Cuboid::generate(int vertexMode, int vertexAttribute, int indexFlag)
    {
        float xLength = mXCount * mXUnit;
        float yLength = mYCount * mYUnit;
        float zLength = mZCount * mZUnit;

        // cuboid will fill z anyway, so skip
        vertexAttribute &= GeoPlane::Attribute_FillZ;

        VertexBufferPtr vertexBuffer = new VertexBuffer(vertexMode, vertexAttribute);
        vertexBuffer->setVertexComponentSize(3);
        GeoPlane* frontBack = HouyiNew GeoPlane(mXCount, mYCount, mXUnit, mYUnit, mFlipV, false);
        frontBack->generate(vertexMode, vertexAttribute, indexFlag);
        VertexBufferPtr pFVB = frontBack->getSurface(0)->getVertexBuffer();
        int frontBackVCnt = pFVB->getPositionLength()/2;

        GeoPlane* topBottom = HouyiNew GeoPlane(mXCount, mZCount, mXUnit, mZUnit, mFlipV, false);
        topBottom->generate(vertexMode, vertexAttribute, indexFlag);
        VertexBufferPtr pTVB = topBottom->getSurface(0)->getVertexBuffer();
        int topBottomVCnt = pTVB->getPositionLength()/2;

        GeoPlane* leftRight = HouyiNew GeoPlane(mZCount, mYCount, mZUnit, mYUnit, mFlipV, false);
        leftRight->generate(vertexMode, vertexAttribute, indexFlag);
        VertexBufferPtr pLVB = leftRight->getSurface(0)->getVertexBuffer();
        int leftRightVCnt = pLVB->getPositionLength()/2;

        int vCnt = (frontBackVCnt + topBottomVCnt + leftRightVCnt) * 2;
        int positionLength = vCnt * 3;
        float* position = new float[positionLength];
        vertexBuffer->setPosition(position, positionLength);
        float* normal = 0;
        if ((vertexAttribute & VertexBuffer::Attribute_Normal) != 0)
        {
            normal = new float[positionLength];
            vertexBuffer->setNormal(normal, positionLength);
        }
        float* texcoord = 0;
        int texLength = vCnt * 2;
        if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
        {
            texcoord = new float[texLength];
            vertexBuffer->setTexcoord(texcoord, texLength);
        }
        int vIdx = 0;
        float* planePosition = pFVB->getPosition();
        float* planeTexcoord = pFVB->getTexcoord();
        // front
        for (int i = 0;i < frontBackVCnt;++i)
        {
            position[vIdx * 3] = planePosition[i * 2];
            position[vIdx * 3 + 1] = planePosition[i * 2 + 1];
            position[vIdx * 3 + 2] = zLength / 2;
            if (normal)
            {
                normal[vIdx * 3] = 0;
                normal[vIdx * 3 + 1] = 0;
                normal[vIdx * 3 + 2] = 1;
            }
            if (texcoord)
            {
                texcoord[vIdx * 2] = planeTexcoord[i * 2];
                texcoord[vIdx * 2 + 1] = planeTexcoord[i * 2 + 1];
            }
            vIdx++;
        }
        // back
        for (int i = 0;i < frontBackVCnt;++i)
        {
            position[vIdx * 3] = planePosition[i * 2];
            position[vIdx * 3 + 1] = planePosition[i * 2 + 1];
            position[vIdx * 3 + 2] = -zLength / 2;
            if (normal)
            {
                normal[vIdx * 3] = 0;
                normal[vIdx * 3 + 1] = 0;
                normal[vIdx * 3 + 2] = -1;
            }
            if (texcoord)
            {
                texcoord[vIdx * 2] = planeTexcoord[i * 2];
                texcoord[vIdx * 2 + 1] = planeTexcoord[i * 2 + 1];
            }
            vIdx++;
        }

        planePosition = pTVB->getPosition();
        planeTexcoord = pTVB->getTexcoord();
        // top
        for (int i = 0;i < topBottomVCnt;++i)
        {
            position[vIdx * 3] = planePosition[i * 2];
            position[vIdx * 3 + 1] = yLength / 2;
            position[vIdx * 3 + 2] = planePosition[i * 2 + 1];
            if (normal)
            {
                normal[vIdx * 3] = 0;
                normal[vIdx * 3 + 1] = 1;
                normal[vIdx * 3 + 2] = 0;
            }
            if (texcoord)
            {
                texcoord[vIdx * 2] = planeTexcoord[i * 2];
                texcoord[vIdx * 2 + 1] = planeTexcoord[i * 2 + 1];
            }
            vIdx++;
        }
        // bottom
        for (int i = 0;i < topBottomVCnt;++i)
        {
            position[vIdx * 3] = planePosition[i * 2];
            position[vIdx * 3 + 1] = -yLength / 2;
            position[vIdx * 3 + 2] = planePosition[i * 2 + 1];
            if (normal)
            {
                normal[vIdx * 3] = 0;
                normal[vIdx * 3 + 1] = -1;
                normal[vIdx * 3 + 2] = 0;
            }
            if (texcoord)
            {
                texcoord[vIdx * 2] = planeTexcoord[i * 2];
                texcoord[vIdx * 2 + 1] = planeTexcoord[i * 2 + 1];
            }
            vIdx++;
        }

        planePosition = pLVB->getPosition();
        planeTexcoord = pLVB->getTexcoord();
        // right
        for (int i = 0;i < leftRightVCnt;++i)
        {
            position[vIdx * 3] = xLength / 2;
            position[vIdx * 3 + 1] = planePosition[i * 2 + 1];
            position[vIdx * 3 + 2] = planePosition[i * 2];
            if (normal)
            {
                normal[vIdx * 3] = 1;
                normal[vIdx * 3 + 1] = 0;
                normal[vIdx * 3 + 2] = 0;
            }
            if (texcoord)
            {
                texcoord[vIdx * 2] = planeTexcoord[i * 2];
                texcoord[vIdx * 2 + 1] = planeTexcoord[i * 2 + 1];
            }
            vIdx++;
        }
        // left
        for (int i = 0;i < leftRightVCnt;++i)
        {
            position[vIdx * 3] = -xLength / 2;
            position[vIdx * 3 + 1] = planePosition[i * 2 + 1];
            position[vIdx * 3 + 2] = planePosition[i * 2];
            if (normal)
            {
                normal[vIdx * 3] = -1;
                normal[vIdx * 3 + 1] = 0;
                normal[vIdx * 3 + 2] = 0;
            }
            if (texcoord)
            {
                texcoord[vIdx * 2] = planeTexcoord[i * 2];
                texcoord[vIdx * 2 + 1] = planeTexcoord[i * 2 + 1];
            }
            vIdx++;
        }

        IndexBufferPtr indexBuffer;
        if (indexFlag != 0)
        {
            indexBuffer = new IndexBuffer();
            if ((indexFlag & Generate_Line_Index) != 0)
            {
                IndexBufferPtr fIB = frontBack->getSurface(0)->getIndexBuffer();
                int fIBCnt = fIB->getLineIndicesLength();
                uint* fIndices = fIB->getIntLineIndices();
                
                IndexBufferPtr tIB = topBottom->getSurface(0)->getIndexBuffer();
                int tIBCnt = tIB->getLineIndicesLength();
                uint* tIndices = tIB->getIntLineIndices();
                
                IndexBufferPtr lIB = leftRight->getSurface(0)->getIndexBuffer();
                int lIBCnt = lIB->getLineIndicesLength();
                uint* lIndices = lIB->getIntLineIndices();
                
                int indexLength = (fIBCnt + tIBCnt + lIBCnt) * 2;
                uint* indices = new uint[indexLength];
                int idx = 0;
                int offset = 0;
                // front
                for (int i = 0;i < fIBCnt;++i, idx++)
                {
                    indices[idx] = fIndices[i];
                }
                offset += frontBackVCnt;
                
                // back
                for (int i = 0;i < fIBCnt;++i, idx++)
                {
                    indices[idx] = fIndices[i] + offset;
                }
                offset += frontBackVCnt;
                
                // top
                for (int i = 0;i < tIBCnt;++i, idx++)
                {
                    indices[idx] = tIndices[i] + offset;
                }
                offset += topBottomVCnt;
                
                // bottom
                for (int i = 0;i < tIBCnt;++i, idx++)
                {
                    indices[idx] = tIndices[i] + offset;
                }
                offset += topBottomVCnt;
                
                // right
                for (int i = 0;i < lIBCnt;++i, idx++)
                {
                    indices[idx] = lIndices[i] + offset;
                }
                offset += leftRightVCnt;
                
                // left
                for (int i = 0;i < lIBCnt;++i, idx++)
                {
                    indices[idx] = lIndices[i] + offset;
                }
                
                indexBuffer->setLineIndices(indices, indexLength);
            }
            
            if ((indexFlag & Generate_Triangle_Index) != 0)
            {
                IndexBufferPtr fIB = frontBack->getSurface(0)->getIndexBuffer();
                int fIBCnt = fIB->getTriangleIndicesLength();
                uint* fIndices = fIB->getIntTriangleIndices();
                
                IndexBufferPtr tIB = topBottom->getSurface(0)->getIndexBuffer();
                int tIBCnt = tIB->getTriangleIndicesLength();
                uint* tIndices = tIB->getIntTriangleIndices();
                
                IndexBufferPtr lIB = leftRight->getSurface(0)->getIndexBuffer();
                int lIBCnt = lIB->getTriangleIndicesLength();
                uint* lIndices = lIB->getIntTriangleIndices();
                
                int indexLength = (fIBCnt + tIBCnt + lIBCnt) * 2;
                uint* indices = new uint[indexLength];
                int idx = 0;
                int offset = 0;
                // front 
                for (int i = 0;i < fIBCnt;++i, idx++)
                {
                    indices[idx] = fIndices[i];
                }
                offset += frontBackVCnt;
                
                // back
                for (int i = 0;i < fIBCnt;++i, idx++)
                {
                    indices[idx] = fIndices[i] + offset;
                }
                offset += frontBackVCnt;
                
                // top
                for (int i = 0;i < tIBCnt;++i, idx++)
                {
                    indices[idx] = tIndices[i] + offset;
                }
                offset += topBottomVCnt;
                
                // bottom
                for (int i = 0;i < tIBCnt;++i, idx++)
                {
                    indices[idx] = tIndices[i] + offset;
                }
                offset += topBottomVCnt;
                
                // right
                for (int i = 0;i < lIBCnt;++i, idx++)
                {
                    indices[idx] = lIndices[i] + offset;
                }
                offset += leftRightVCnt;
                
                // left
                for (int i = 0;i < lIBCnt;++i, idx++)
                {
                    indices[idx] = lIndices[i] + offset;
                }
                
                indexBuffer->setTriangleIndices(indices, indexLength);
            }
        }

        SurfacePtr surface = new Surface(vertexBuffer, indexBuffer);
        addSurface(surface);

        HouyiDelete (frontBack);
        HouyiDelete (topBottom);
        HouyiDelete (leftRight);
        return true;
    }
}
