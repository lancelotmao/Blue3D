#ifndef HouyiIndexBuffer_H
#define HouyiIndexBuffer_H

#include "RefCount.h"
#include "SmartPtr.h"

namespace Houyi
{
    class IndexBuffer: public RefCountBase
    {
    public:
        IndexBuffer() : mShortLineIndices(0), mIntLineIndices(0), mLineIndicesLength(0), mShortTriangleIndices(0),
        mIntTriangleIndices(0), mTriangleIndicesLength(0), mLineIndexSize(4), mTriangleIndexSize(4)
        {
        }

        ~IndexBuffer()
        {
            clearLineIndices();
            clearTriangleIndices();
        }

        inline void setLineIndices(ushort* indices, int length)
        {
            clearLineIndices();
            mShortLineIndices = indices;
            mLineIndicesLength = length;
            mLineIndexSize = 2;
        }

        inline void setLineIndices(uint* indices, int length)
        {
            clearLineIndices();
            mIntLineIndices = indices;
            mLineIndicesLength = length;
            mLineIndexSize = 4;
        }

        inline ushort* getShortLineIndices()
        {
            return mShortLineIndices;
        }

        inline uint* getIntLineIndices()
        {
            return mIntLineIndices;
        }

        inline int getLineIndicesLength()
        {
            return mLineIndicesLength;
        }

        inline void setTriangleIndices(ushort* indices, int length)
        {
            clearTriangleIndices();
            mShortTriangleIndices = indices;
            mTriangleIndicesLength = length;
            mTriangleIndexSize = 2;
        }
        
        inline void setTriangleIndices(uint* indices, int length)
        {
            clearTriangleIndices();
            mIntTriangleIndices = indices;
            mTriangleIndicesLength = length;
            mTriangleIndexSize = 4;
        }

        inline ushort* getShortTriangleIndices()
        {
            return mShortTriangleIndices;
        }

        inline uint* getIntTriangleIndices()
        {
            return mIntTriangleIndices;
        }

        inline int getTriangleIndicesLength()
        {
            return mTriangleIndicesLength;
        }
        
        inline int getLineIndexSize()
        {
            return mLineIndexSize;
        }

        inline int getTriangleIndexSize()
        {
            return mTriangleIndexSize;
        }

        inline void clearLineIndices()
        {
            if (mShortLineIndices)
            {
                delete[] mShortLineIndices;
                mShortLineIndices = 0;
            }

            if (mIntLineIndices)
            {
                delete[] mIntLineIndices;
                mIntLineIndices = 0;
            }
            mLineIndicesLength = 0;
        }

        inline void clearTriangleIndices()
        {
            if (mShortTriangleIndices)
            {
                delete[] mShortTriangleIndices;
                mShortTriangleIndices = 0;
            }

            if (mIntTriangleIndices)
            {
                delete[] mIntTriangleIndices;
                mIntTriangleIndices = 0;
            }
            mTriangleIndicesLength = 0;
        }

    private:
        ushort* mShortLineIndices;
        uint* mIntLineIndices;
        int mLineIndicesLength;

        ushort* mShortTriangleIndices;
        uint* mIntTriangleIndices;
        int mTriangleIndicesLength;
        
        // 2 = short, 4 = int
        int mLineIndexSize;
        int mTriangleIndexSize;
    };

    typedef SmartPtr<IndexBuffer> IndexBufferPtr;
}

#endif
