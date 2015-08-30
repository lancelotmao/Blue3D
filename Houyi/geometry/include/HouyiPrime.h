#ifndef Houyi_HouyiPrime_h
#define Houyi_HouyiPrime_h

#include "HouyiGeometry.h"

namespace Houyi
{
    class Prime : public Geometry
    {
    public:
        Prime(int count) : mVertexCount(count), mType(0)
        {
            if (mType == 0 || mType == 1)
            {
                mLoopCount = 6;
            }
            else if (mType == 2)
            {
                mLoopCount = 3;
            }
        }

        virtual ~Prime(){}
        virtual bool generate(int vertexMode = VertexBuffer::ENormal, int vertexAttribute = VertexBuffer::Attribute_Texcoord,
                        int indexFlag = Generate_Line_Index | Generate_Triangle_Index)
        {
            int vertexCount = mVertexCount;

            int prime[vertexCount];
            int j = 0;
            int i = 2;
            while(true)
            {
                if (isPrime(i))
                {
                    prime[j++] = i;
                }
                if (j == vertexCount - 1)
                {
                    break;
                }
                else
                {
                    i++;
                }
            }

            VertexBufferPtr vertexBuffer = new VertexBuffer(VertexBuffer::ENormal, 0);
            vertexBuffer->setVertexComponentSize(3);

            float* position = new float[vertexCount * 3];
            vertexBuffer->setPosition(position, vertexCount * 3);

            Vector3 pos(1, 1, 1);

            int flag = 0;

            for (int i = 0;i < vertexCount;++i)
            {
                if (mType == 0)
                {
                    if (flag == 0)
                    {
                        pos.x -= prime[i];
                    }
                    else if (flag == 1)
                    {
                        pos.y -= prime[i];
                    }
                    else if (flag == 2)
                    {
                        pos.z -= prime[i];
                    }
                    else if (flag == 3)
                    {
                        pos.x += prime[i];
                    }
                    else if (flag == 4)
                    {
                        pos.y += prime[i];
                    }
                    else if (flag == 5)
                    {
                        pos.z += prime[i];
                    }
                }
                else if (mType == 1)
                {
                    if (flag == 0)
                    {
                        pos.x = -prime[i];
                    }
                    else if (flag == 1)
                    {
                        pos.y = -prime[i];
                    }
                    else if (flag == 2)
                    {
                        pos.z = -prime[i];
                    }
                    else if (flag == 3)
                    {
                        pos.x = prime[i];
                    }
                    else if (flag == 4)
                    {
                        pos.y = prime[i];
                    }
                    else if (flag == 5)
                    {
                        pos.z = prime[i];
                    }
                }
                else if (mType == 2)
                {
                    if (flag == 0)
                    {
                        pos.x = prime[i];
                    }
                    else if (flag == 1)
                    {
                        pos.y = prime[i];
                    }
                    else if (flag == 2)
                    {
                        pos.z = prime[i];
                    }
                }

                position[i * 3] = 0.1 * pos.x;
                position[i * 3 + 1] = 0.1 * pos.y;
                position[i * 3 + 2] = 0.1 * pos.z;

                flag++;
                if (flag == mLoopCount)
                {
                    flag = 0;
                }
            }

            for (int i = 0;i < vertexCount;++i)
            {
                LOGD("p = %f, %f, %f", position[i * 3], position[i * 3 + 1], position[i * 3 + 2]);
            }

            SurfacePtr surface = new Surface(vertexBuffer);
            addSurface(surface);
            return true;
        }

    private:
        int mVertexCount;
        int mType;
        int mLoopCount;

        bool isPrime(int n)
        {
            int b = (int)(sqrt((float)n));
            for (int i = 2;i <= b;++i)
            {
                if (n % i == 0)
                {
                    return false;
                }
            }

            return true;
        }
    };
}

#endif
