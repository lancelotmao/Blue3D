#include "HouyiVertexBuffer.h"
#include "HouyiCapability.h"
#include "HouyiRoot.h"
#include "HouyiRenderer.h"

namespace Houyi
{
    VertexBuffer::VertexBuffer(int vertexMode, int attriFlag) :
	mVertexComponentSize(3), mPosition(0), mPositionLength(0),
	mNormal(0), mNormalLength(0), mTexcoord(0), mTexcoordLength(0),
	mJointIndex(0), mJointIndexLength(0), mJointWeight(0), mJointWeightLength(0),
    mTangent(0), mTangentLength(0), mBinormal(0), mBinormalLength(0),
    mColor(0), mColorLength(0), mGeoPos(0), mIsBound(false),
	mVertexMode(vertexMode), mAttributeFlag(attriFlag), mVBOs(0)
    {
        mUseVBO = Capability::useVBO();
    }

    VertexBuffer::~VertexBuffer()
    {
//        Root* root = Root::getInstance();
//        if (mIsBound && !root->mAutoFinalize)
//        {
//            Renderer* renderer = root->getRenderer();
//            if (renderer)
//            {
//                renderer->deleteHardwareBuffer(getVBOCount(), getVBOs());
//            }
//        }

        if (mPosition)
        {
            delete[] mPosition;
            mPosition = 0;
        }

        if (mNormal)
        {
            delete[] mNormal;
            mNormal = 0;
        }

        if (mTexcoord)
        {
            delete[] mTexcoord;
            mTexcoord = 0;
        }

        if (mJointIndex)
        {
            delete[] mJointIndex;
            mJointIndex = 0;
        }

        if (mJointWeight)
        {
            delete[] mJointWeight;
            mJointWeight = 0;
        }

        if (mGeoPos)
        {
            delete[] mGeoPos;
            mGeoPos = 0;
        }

        if (mTangent)
        {
            delete[] mTangent;
            mTangent = 0;
        }

        if (mBinormal)
        {
            delete[] mBinormal;
            mBinormal = 0;
        }

        if (mColor)
        {
            delete[] mColor;
            mColor = 0;
        }
        
        if (mVBOs)
        {
            delete[] mVBOs;
            mVBOs = 0;
        }
    }

    float* VertexBuffer::getPosition()
    {
        return mPosition;
    }

    void VertexBuffer::setPosition(float* position, int positionLength)
    {
        if (mPosition)
        {
            delete[] mPosition;
        }
        mPosition = position;
        mPositionLength = positionLength;
        if (mGeoPos)
        {
            delete[] mGeoPos;
            mGeoPos = 0;
        }
        mGeoPos = new float[positionLength];
        memcpy(mGeoPos, mPosition, positionLength * sizeof(float));
        setBind(false);
    }

    int VertexBuffer::getPositionLength()
    {
        return mPositionLength;
    }

    float* VertexBuffer::getNormal()
    {
        return mNormal;
    }

    void VertexBuffer::setNormal(float* normal, int normalLength)
    {
        if (mNormal)
        {
            delete[] mNormal;
        }
        mNormal = normal;
        mNormalLength = normalLength;
        if (normal)
        {
            mAttributeFlag |= Attribute_Normal;
        }
        setBind(false);
    }

    int VertexBuffer::getNormalLength()
    {
        return mNormalLength;
    }

    float* VertexBuffer::getTexcoord()
    {
        return mTexcoord;
    }

    void VertexBuffer::setTexcoord(float* texcoord, int texcoordLength)
    {
        if (mTexcoord)
        {
            delete[] mTexcoord;
        }
        mTexcoord = texcoord;
        mTexcoordLength = texcoordLength;
        if (texcoord)
        {
            mAttributeFlag |= Attribute_Texcoord;
        }
        setBind(false);
    }

    int VertexBuffer::getTexcoordLength()
    {
        return mTexcoordLength;
    }

    float* VertexBuffer::getJointIndex()
    {
        return mJointIndex;
    }

    void VertexBuffer::setJointIndex(float* jointIndex, int length)
    {
        if (mJointIndex)
        {
            delete[] mJointIndex;
        }
        
        mJointIndex = jointIndex;
        mJointIndexLength = length;
        if (jointIndex)
        {
            mAttributeFlag |= Attribute_Skeleton;
        }
        setBind(false);
    }

    int VertexBuffer::getJointIndexLength()
    {
        return mJointIndexLength;
    }

    float* VertexBuffer::getJointWeight()
    {
        return mJointWeight;
    }

    void VertexBuffer::setJointWeight(float* jointWeight, int length)
    {
        if (mJointWeight)
        {
            delete[] mJointWeight;
        }
        mJointWeight = jointWeight;
        mJointWeightLength = length;
        if (jointWeight)
        {
            mAttributeFlag |= Attribute_Skeleton;
        }
        setBind(false);
    }

    int VertexBuffer::getJointWeightLength()
    {
        return mJointWeightLength;
    }

    float* VertexBuffer::getGeoPos()
    {
        return mGeoPos;
    }

    void VertexBuffer::setGeoPos(float* geoPos)
    {
        mGeoPos = geoPos;
    }

    float* VertexBuffer::getTangent()
    {
        return mTangent;
    }

    void VertexBuffer::setTangent(float* tangent, int length)
    {
        if (mTangent)
        {
            delete[] mTangent;
        }
        mTangent = tangent;
        mTangentLength = length;
        if (tangent)
        {
            mAttributeFlag |= Attribute_Tangent;
        }
        setBind(false);
    }

    int VertexBuffer::getTangentLength()
    {
        return mTangentLength;
    }

    float* VertexBuffer::getBinormal()
    {
        return mBinormal;
    }

    void VertexBuffer::setBinormal(float* binormal, int length)
    {
        if (mBinormal)
        {
            delete[] mBinormal;
        }
        mBinormal = binormal;
        mBinormalLength = length;
        if (binormal)
        {
            mAttributeFlag |= Attribute_Binormal;
        }
        setBind(false);
    }

    int VertexBuffer::getBinormalLength()
    {
        return mBinormalLength;
    }

    float* VertexBuffer::getColor()
    {
        return mColor;
    }

    void VertexBuffer::setColor(float* color, int length) {
        if (mColor)
        {
            delete[] mColor;
        }
        mColor = color;
        mColorLength = length;
        if (color)
        {
            mAttributeFlag |= Attribute_Color;
        }
        setBind(false);
    }

    int VertexBuffer::getColorLength()
    {
        return mColorLength;
    }

    int VertexBuffer::getVertexComponentSize()
    {
        return mVertexComponentSize;
    }

    void VertexBuffer::setVertexComponentSize(int size)
    {
        mVertexComponentSize = size;
    }

    void VertexBuffer::setBind(bool bind)
    {
        mIsBound = bind;
    }

    bool VertexBuffer::isBound()
    {
        return mIsBound;
    }

    int VertexBuffer::getVertexMode()
    {
        return mVertexMode;
    }

    void VertexBuffer::setVertexMode(int mode)
    {
        mVertexMode = mode;
    }

    int VertexBuffer::getAttributeFlag()
    {
        return mAttributeFlag;
    }

    void VertexBuffer::setAttributeFlag(int flag)
    {
        mAttributeFlag = flag;
    }

    int VertexBuffer::getVBOCount()
    {
        int count = 0;
        if (mPosition) count++;
        if (mNormal) count++;
        if (mTexcoord) count++;
        if (mJointIndex) count++;
        if (mJointWeight) count++;
        if (mTangent) count++;
        if (mBinormal) count++;
        if (mColor) count++;
        return count;
    }
}
