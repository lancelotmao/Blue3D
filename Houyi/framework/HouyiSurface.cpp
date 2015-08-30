#include "HouyiSurface.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"

namespace Houyi
{
    Surface::Surface()
    {
        init();
    }

    Surface::Surface(VertexBufferPtr vb, IndexBufferPtr ib)
    {
        mRawVertexBuffer = vb;
        mIndexBuffer = ib;
        init();
    }

    void Surface::init()
    {
    	mBufferHandle = 0;
        mMaterial = 0;
        mLineWidth = 2; // seldom case to be 1
        mParentMesh = 0;
        mBufferValid = false;
        mRenderMode = Const::ERenderModeTriangle;
        mPointSize = 5;
    }

    Surface::~Surface()
    {
        if (mBufferValid)
        {
            Root* root = Root::getInstance();
            if (!root->mAutoFinalize)
            {
                Renderer* renderer = root->getRenderer();
                if (renderer)
                {
                    uint handle[1];
                    handle[0] = mBufferHandle;
                    renderer->deleteHardwareBuffer(1, handle);
                }
            }
        }
    }

    VertexBufferPtr Surface::getVertexBuffer()
    {
        VertexBufferPtr pvb;
        if (mParentMesh)
        {
            pvb = mParentMesh->getVertexBuffer();
        }
        return pvb ? pvb : mRawVertexBuffer;
    }

    void Surface::setRawVertexBuffer(VertexBufferPtr vb)
    {
        mRawVertexBuffer = vb;
    }

    IndexBufferPtr Surface::getIndexBuffer()
    {
        if (mParentMesh)
        {
            return mParentMesh->getMode() == Mesh::ECompact ? mIndexBuffer : mExpandedIndex;
        }
        return mIndexBuffer;
    }

    void Surface::setMesh(Mesh* mesh)
    {
        mParentMesh = mesh;
    }

    Mesh* Surface::getMesh()
    {
        return mParentMesh;
    }
}
