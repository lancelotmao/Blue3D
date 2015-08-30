#ifndef __Houyi__HouyiSurface__
#define __Houyi__HouyiSurface__

#include "RefCount.h"
#include "SmartPtr.h"
#include "HouyiConst.h"
#include "HouyiVertexBuffer.h"
#include "HouyiIndexBuffer.h"
#include "HouyiMaterial.h"

namespace Houyi
{
    class Mesh;

    class Surface : public RefCountBase
    {
    public:
        Surface();
        Surface(VertexBufferPtr vb, IndexBufferPtr ib = 0);
        ~Surface();

        inline VertexBufferPtr getRawVertexBuffer()
        {
            return mRawVertexBuffer;
        }

        void setRawVertexBuffer(VertexBufferPtr vb);

        VertexBufferPtr getVertexBuffer();

        IndexBufferPtr getIndexBuffer();

        inline void setIndexBuffer(IndexBufferPtr ib)
        {
            mIndexBuffer = ib;
        }

        inline void setExpandedBuffer(IndexBufferPtr ib)
        {
            mExpandedIndex = ib;
        }

        inline IndexBufferPtr getWireFrameIndexBuffer()
        {
            return mWireFrameIndex;
        }

        inline void setBufferHandle(unsigned int handle)
        {
        	mBufferHandle = handle;
        }

        inline unsigned int getBufferHandle()
        {
        	return mBufferHandle;
        }

        inline void setWireFrameIndexBuffer(IndexBufferPtr ib)
        {
            mWireFrameIndex = ib;
        }

        inline Material* getMaterial()
        {
            return mMaterial;
        }

        inline void setMaterial(Material* material)
        {
            mMaterial = material;
        }

        inline int getLineWidth()
        {
            return mLineWidth;
        }

        inline void setLineWidth(int lineWidth)
        {
            mLineWidth = lineWidth;
        }

        inline int getLineCount()
        {
            return mIndexBuffer ? mIndexBuffer->getLineIndicesLength()/2 :
                    getVertexBuffer() ? getVertexBuffer()->getPositionLength() / 3 : 0;
        }

        inline int getTriangleCount()
        {
            return mIndexBuffer ? mIndexBuffer->getTriangleIndicesLength()/3 :
                    getVertexBuffer() ? getVertexBuffer()->getPositionLength() / 9 : 0;
        }

        void setMesh(Mesh* mesh);
        Mesh* getMesh();

        inline bool isIndexBufferBound()
        {
            return mBufferHandle != 0;
        }

        inline void unBindIndexBuffer()
        {
            mBufferHandle = 0;
            mBufferValid = false;
        }

        inline bool isBufferValid()
        {
            return mBufferValid;
        }

        inline void setBufferValid(bool v)
        {
            mBufferValid = v;
        }

        inline int getRenderMode()
        {
            return mRenderMode;
        }

        inline void setRenderMode(int renderMode)
        {
            mRenderMode = renderMode;
        }

        inline float getPointSize()
        {
            return mPointSize;
        }

        inline void setPointSize(float pointSize)
        {
            mPointSize = pointSize;
        }

    protected:
        VertexBufferPtr mRawVertexBuffer;
        IndexBufferPtr mIndexBuffer;
        IndexBufferPtr mExpandedIndex;
        IndexBufferPtr mWireFrameIndex;
        unsigned int mBufferHandle;

        Material* mMaterial;
        int mLineWidth;
        Mesh* mParentMesh;
        bool mBufferValid;
        int mRenderMode;
        float mPointSize;

    private:
        void init();
    };

    typedef SmartPtr<Surface> SurfacePtr;
}

#endif
