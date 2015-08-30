#include "HouyiCircle.h"

namespace Houyi
{
    Circle::Circle(float r, int dtheta, bool generateNow) : mR(r), mDTheta(dtheta)
    {
        if (generateNow)
        {
            generate();
        }
    }

    bool Circle::generate(int vertexMode, int vertexAttribute, int indexFlag)
    {
        float r = mR;
        float dtheta = mDTheta;

        vector<Vector3> vCircle;
        for (int theta = 0;theta < 360;theta+=dtheta)
        {
            float x = r * cos(Math::DegreesToRadians(theta));
            float y = r * sin(Math::DegreesToRadians(theta));
            vCircle.push_back(Vector3(x, y, 0));
        }

        VertexBufferPtr vb = new VertexBuffer();
        IndexBufferPtr indexBuffer;
        if (indexFlag != 0)
        {
            indexBuffer = new IndexBuffer();
        }
        float* pos = 0;
        float* normal = 0;
        ushort* triangleIndices = 0;
        int vcnt = 0;
        if (vertexMode == VertexBuffer::EExpandAsTriangles)
        {
            vcnt = vCircle.size() * 3;
            pos = new float[vcnt * 3];

            for (int i = 0;i < vCircle.size();i++)
            {
                Vector3& v1 = vCircle.at(i);
                pos[i * 9] = v1.x;
                pos[i * 9 + 1] = v1.y;
                pos[i * 9 + 2] = v1.z;

                pos[i * 9 + 3] = 0;
                pos[i * 9 + 4] = 0;
                pos[i * 9 + 5] = 0;

                Vector3 v3;
                if (i < vCircle.size() - 1)
                {
                    v3 = vCircle.at(i + 1);
                }
                else
                {
                    v3 = vCircle.at(0);
                }
                pos[i * 9 + 6] = v3.x;
                pos[i * 9 + 7] = v3.y;
                pos[i * 9 + 8] = v3.z;
            }
        }
        else
        {
            vcnt = vCircle.size() + 1;
            if ((indexFlag & Generate_Triangle_Index) != 0)
            {
                int length = vCircle.size() * 3;
                triangleIndices = new ushort[length];
                indexBuffer->setTriangleIndices(triangleIndices, length);
            }
            pos = new float[vcnt * 3];
            pos[0] = 0;
            pos[1] = 0;
            pos[2] = 0;
            for (int i = 0;i < vCircle.size();i++)
            {
                Vector3& v = vCircle.at(i);
                pos[(i+1) * 3] = v.x;
                pos[(i+1) * 3 + 1] = v.y;
                pos[(i+1) * 3 + 2] = v.z;
                if (triangleIndices)
                {
                    triangleIndices[i * 3] = i+1;
                    triangleIndices[i * 3 + 1] = 0;
                    int idx = i < vCircle.size() - 1 ? i + 2 : 1;
                    triangleIndices[i * 3 + 2] = idx;
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

        SurfacePtr surface = new Surface(vb, indexBuffer);
        addSurface(surface);
        return true;
    }
}
