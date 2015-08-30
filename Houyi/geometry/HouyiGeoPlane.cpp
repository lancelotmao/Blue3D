#include "HouyiGeoPlane.h"

namespace Houyi
{
    bool GeoPlane::generate(int vertexMode, int vertexAttribute, int indexFlag)
    {
        int horizontalCount = mHorizontalCount;
        int verticalCount = mVerticalCount;
        float horizontalUnit = mHorizontalUnit;
        float verticalUnit = mVerticalUnit;
        if (horizontalCount <= 0 || verticalCount <= 0 || horizontalUnit <= 0 || verticalUnit <= 0)
        {
            LOGW("Houyi::Plane::generate illegal args. horizontalCount = %d, verticalCount = %d\n", horizontalCount, verticalCount);
            return false;
        }

        float startX = -horizontalCount / 2.0f * horizontalUnit;
        float startY = -verticalCount / 2.0f * verticalUnit;
        float unitU = 1.0 / horizontalCount;
        float unitV = 1.0 / verticalCount;
        VertexBufferPtr vertexBuffer = new VertexBuffer(vertexMode, vertexAttribute);
        if ((vertexAttribute & Attribute_FillZ) != 0)
        {
            vertexBuffer->setVertexComponentSize(3);
        }
        else
        {
            vertexBuffer->setVertexComponentSize(2);
        }

        int vertexCount = 0;
        if (vertexMode == VertexBuffer::ENormal)
        {
            vertexCount = (horizontalCount + 1) * (verticalCount + 1);
        }
        else if (vertexMode == VertexBuffer::EExpandAsLines)
        {
            // TODO
        }
        else
        {
            vertexCount = (horizontalCount) * (verticalCount) * 6;
        }

        int componentSize = vertexBuffer->getVertexComponentSize();
        int positionLength = vertexCount * componentSize;
        float* position = new float[positionLength];
        if (!position)
        {
            LOGD("Plane::generate Out of Memory\n");
            return false;
        }
        memset(position, 0, positionLength * sizeof(float));

        float* normal = 0;
        int normalLength = 0;
        if ((vertexAttribute & VertexBuffer::Attribute_Normal) != 0)
        {
            normalLength = vertexCount * 3;
            normal = new float[normalLength];
            if (!normal)
            {
                LOGD("Plane::generate Out of Memory\n");
                return false;
            }
            vertexBuffer->setNormal(normal, normalLength);
        }

        float* texcoord = 0;
        int texcoordLength = 0;
        if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
        {
            texcoordLength = positionLength;
            texcoord = new float[texcoordLength];
            if (!texcoord)
            {
                LOGD("Plane::generate Out of Memory\n");
                return false;
            }
            vertexBuffer->setTexcoord(texcoord, texcoordLength);
        }

//		LOGD("Plane data length = %d, startX = %f, startY = %f\n", positionLength, startX, startY);
//		LOGD("Plane unitU = %f, unitV = %f\n", unitU, unitV);

        if (vertexMode == VertexBuffer::ENormal)
        {
            for (int i = 0; i <= verticalCount; ++i)
            {
                for (int j = 0; j <= horizontalCount; ++j)
                {
                    int vertexIndex = i * (horizontalCount + 1) + j;
                    position[vertexIndex * componentSize] = startX + j * horizontalUnit;
                    position[vertexIndex * componentSize + 1] = startY + i * verticalUnit;

                    if ((vertexAttribute & VertexBuffer::Attribute_Normal) != 0)
                    {
                        normal[vertexIndex * 3] = 0;
                        normal[vertexIndex * 3 + 1] = 0;
                        normal[vertexIndex * 3 + 2] = 1;
                    }

                    if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
                    {
                        texcoord[vertexIndex * 2] = j * unitU;
                        texcoord[vertexIndex * 2 + 1] = i * unitV;
                    }
                }
            }
        }
        else if (vertexMode == VertexBuffer::EExpandAsLines)
        {
            // TODO
        }
        else
        {
            int vidx = 0;
            int tidx = 0;
            for (int y = 0; y < verticalCount; ++y)
            {
                for (int x = 0; x < horizontalCount; ++x)
                {
                    position[vidx++] = startX + horizontalUnit * x;
                    position[vidx++] = startY + verticalUnit * y;
                    if (componentSize == 3)
                        position[vidx++] = 0;
                    position[vidx++] = startX + horizontalUnit * (x + 1);
                    position[vidx++] = startY + verticalUnit * y;
                    if (componentSize == 3)
                        position[vidx++] = 0;
                    position[vidx++] = startX + horizontalUnit * x;
                    position[vidx++] = startY + verticalUnit * (y + 1);
                    if (componentSize == 3)
                        position[vidx++] = 0;

                    position[vidx++] = startX + horizontalUnit * (x + 1);
                    position[vidx++] = startY + verticalUnit * y;
                    if (componentSize == 3)
                        position[vidx++] = 0;
                    position[vidx++] = startX + horizontalUnit * (x + 1);
                    position[vidx++] = startY + verticalUnit * (y + 1);
                    if (componentSize == 3)
                        position[vidx++] = 0;
                    position[vidx++] = startX + horizontalUnit * x;
                    position[vidx++] = startY + verticalUnit * (y + 1);
                    if (componentSize == 3)
                        position[vidx++] = 0;

                    if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0)
                    {
                        texcoord[tidx++] = unitU * x;
                        texcoord[tidx++] = unitV * y;
                        texcoord[tidx++] = unitU * (x + 1);
                        texcoord[tidx++] = unitV * y;
                        texcoord[tidx++] = unitU * x;
                        texcoord[tidx++] = unitV * (y + 1);

                        texcoord[tidx++] = unitU * (x + 1);
                        texcoord[tidx++] = unitV * y;
                        texcoord[tidx++] = unitU * (x + 1);
                        texcoord[tidx++] = unitV * (y + 1);
                        texcoord[tidx++] = unitU * x;
                        texcoord[tidx++] = unitV * (y + 1);
                    }
                }
            }

            if ((vertexAttribute & VertexBuffer::Attribute_Normal) != 0)
            {
                for (int i = 0;i < vertexCount;++i)
                {
                    normal[i * 3] = 0;
                    normal[i * 3 + 1] = 0;
                    normal[i * 3 + 2] = 1;
                }
            }
        }

        IndexBufferPtr indexBuffer;
        if (indexFlag != 0)
        {
            indexBuffer = new IndexBuffer();
        }

        if ((indexFlag & Generate_Line_Index) != 0)
        {
            int length = (horizontalCount * (verticalCount + 1) + verticalCount * (horizontalCount + 1)) * 2;
            uint* indices = new uint[length];
            indexBuffer->setLineIndices(indices, length);

            int index = 0;
            for (int i = 0; i <= verticalCount; ++i)
            {
                for (int j = 0; j < horizontalCount; ++j)
                {
                    indices[index++] = (GLushort)(i * (horizontalCount + 1) + j);
                    indices[index++] = (GLushort)(i * (horizontalCount + 1) + j + 1);
                }
            }

            for (int i = 0; i <= horizontalCount; ++i)
            {
                for (int j = 0; j < verticalCount; ++j)
                {
                    indices[index++] = (GLushort)(j * (horizontalCount + 1) + i);
                    indices[index++] = (GLushort)((j + 1) * (horizontalCount + 1) + i);
                }
            }
        }

        if ((indexFlag & Generate_Triangle_Index) != 0)
        {
            int triangleIndex = 0;
            int length = horizontalCount * verticalCount * 6;
            if (length < 65535 && currentPlatform() == PLATFORM_ANDROID)
            {
                // some android device like S3 does not support INT index
                ushort* indices = new ushort[length];
                indexBuffer->setTriangleIndices(indices, length);
                for (int i = 0; i < verticalCount; ++i)
                {
                    for (int j = 0; j < horizontalCount; ++j)
                    {
                        indices[triangleIndex++] = (GLushort)(i * (horizontalCount + 1) + j);
                        indices[triangleIndex++] = (GLushort)(i * (horizontalCount + 1) + j + 1);
                        indices[triangleIndex++] = (GLushort)((i + 1) * (horizontalCount + 1) + j);

                        indices[triangleIndex++] = (GLushort)(i * (horizontalCount + 1) + j + 1);
                        indices[triangleIndex++] = (GLushort)((i + 1) * (horizontalCount + 1) + j + 1);
                        indices[triangleIndex++] = (GLushort)((i + 1) * (horizontalCount + 1) + j);
                    }
                }
            }
            else
            {
                uint* indices = new uint[length];
                indexBuffer->setTriangleIndices(indices, length);
                for (int i = 0; i < verticalCount; ++i)
                {
                    for (int j = 0; j < horizontalCount; ++j)
                    {
                        indices[triangleIndex++] = i * (horizontalCount + 1) + j;
                        indices[triangleIndex++] = i * (horizontalCount + 1) + j + 1;
                        indices[triangleIndex++] = (i + 1) * (horizontalCount + 1) + j;
                        
                        indices[triangleIndex++] = i * (horizontalCount + 1) + j + 1;
                        indices[triangleIndex++] = (i + 1) * (horizontalCount + 1) + j + 1;
                        indices[triangleIndex++] = (i + 1) * (horizontalCount + 1) + j;
                    }
                }
            }
        }

        if ((vertexAttribute & VertexBuffer::Attribute_Texcoord) != 0 && mFlipV)
        {
            for (int i = 1; i < texcoordLength; i += 2)
            {
                texcoord[i] = 1 - texcoord[i];
            }
        }

        vertexBuffer->setPosition(position, positionLength);
        
        SurfacePtr surface = new Surface(vertexBuffer, indexBuffer);
        addSurface(surface);
        return true;
    }
}
