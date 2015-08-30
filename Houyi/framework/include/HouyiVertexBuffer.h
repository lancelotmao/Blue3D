#ifndef HouyiVertexBuffer_H
#define HouyiVertexBuffer_H

#include "RefCount.h"
#include "SmartPtr.h"
#include "HouyiVector2.h"
#include "HouyiVector3.h"
#include "HouyiVector4.h"

namespace Houyi
{
    class VertexData
    {
    public:
        VertexData() : normal(0, 0, 0), texcoord(0, 0),
        boneIndex(0, 0, 0, 0), boneWeight(0, 0, 0, 0), tangent(0, 0, 0)
        {
        }
        Vector3 position;
        Vector3 normal;
        Vector2 texcoord;
        Vector4 boneIndex;
        Vector4 boneWeight;
        Vector3 tangent;
        Vector3 binormal;
        bool operator==(const VertexData& other)
        {
            return position == other.position
                    && normal == other.normal && texcoord == other.texcoord
                    && boneIndex == other.boneIndex && boneWeight == other.boneWeight
                    && tangent == other.tangent;
        }
        bool operator!=(const VertexData& other)
        {
            return !(*this == other);
        }
    };

    class VertexBuffer: public RefCountBase
    {
    public:
        enum VertexMode
        {
            ENormal, EExpandAsLines, EExpandAsTriangles
        };

        static const int Attribute_Texcoord = 1;
        static const int Attribute_Normal = 1 << 1;
        static const int Attribute_Skeleton = 1 << 2;
        static const int Attribute_Tangent = 1 << 3;
        static const int Attribute_Binormal = 1 << 4;
        static const int Attribute_Color = 1 << 5;

        ~VertexBuffer();
        VertexBuffer(int vertexMode = ENormal, int attriFlag = 0);

        float* getPosition();
        void setPosition(float* position, int positionLength);
        int getPositionLength();

        float* getNormal();
        void setNormal(float* normal, int normalLength);
        int getNormalLength();

        float* getTexcoord();
        void setTexcoord(float* texcoord, int texcoordLength);
        int getTexcoordLength();

        float* getJointIndex();
        void setJointIndex(float* jointIndex, int length);
        int getJointIndexLength();

        float* getJointWeight();
        void setJointWeight(float* jointWeight, int length);
        int getJointWeightLength();

        float* getGeoPos();
        void setGeoPos(float* geoPos);

        float* getTangent();
        void setTangent(float* tangent, int length);
        int getTangentLength();

        float* getBinormal();
        void setBinormal(float* binormal, int length);
        int getBinormalLength();

        float* getColor();
        void setColor(float* color, int length);
        int getColorLength();

        int getVertexComponentSize();
        void setVertexComponentSize(int size);

        void setBind(bool bind);
        bool isBound();

        int getVertexMode();
        void setVertexMode(int mode);

        int getAttributeFlag();
        void setAttributeFlag(int flag);
        
        int getVBOCount();
        
        inline void setPositionHandle(unsigned int handle)
        {
            mPositionHandle = handle;
        }
        
        inline unsigned int getPositionHandle()
        {
            return mPositionHandle;
        }

        inline void setTextureHandle(unsigned int handle)
        {
            mTextureHandle = handle;
        }
        
        inline unsigned int getTextureHandle()
        {
            return mTextureHandle;
        }
        
        inline void setNormalHandle(unsigned int handle)
        {
            mNormalHandle = handle;
        }
        
        inline unsigned int getNormalHandle()
        {
            return mNormalHandle;
        }
        
        inline void setJointIndexHandle(unsigned int handle)
        {
            mJointIndexHandle = handle;
        }
        
        inline unsigned int getJointIndexHandle()
        {
            return mJointIndexHandle;
        }
        
        inline void setJointWeightHandle(unsigned int handle)
        {
            mJointWeightHandle = handle;
        }
        
        inline unsigned int getJointWeightHandle()
        {
            return mJointWeightHandle;
        }
        
        inline void setTangentHandle(unsigned int handle)
        {
            mTangentHandle = handle;
        }

        inline unsigned int getTangentHandle()
        {
            return mTangentHandle;
        }

        inline void setBinormalHandle(unsigned int handle)
        {
            mBinoramlHandle = handle;
        }

        inline unsigned int getBinormalHandle()
        {
            return mBinoramlHandle;
        }

        inline void setColorHandle(unsigned int handle)
        {
            mColorHandle = handle;
        }

        inline unsigned int getColorHandle()
        {
            return mColorHandle;
        }

        inline bool useVBO()
        {
            return mUseVBO;
        }

        inline void setUseVBO(bool useVBO)
        {
            mUseVBO = useVBO;
        }
        
        inline int getVertexCount()
        {
            return mPositionLength / mVertexComponentSize;
        }

        inline void setVBOs(int count, uint* vbos)
        {
            if (mVBOs)
            {
                delete[] mVBOs;
            }
            
            mVBOs = new uint[count];
            memcpy(mVBOs, vbos, count * sizeof(uint));
        }

        inline uint* getVBOs()
        {
            return mVBOs;
        }

    private:
        // currently for position and normal only
        int mVertexComponentSize;

        float* mPosition;
        int mPositionLength;

        float* mNormal;
        int mNormalLength;

        float* mTexcoord;
        int mTexcoordLength;

        float* mJointIndex;
        int mJointIndexLength;

        float* mJointWeight;
        int mJointWeightLength;

        float* mTangent;
        int mTangentLength;

        float* mBinormal;
        int mBinormalLength;

        float* mColor;
        int mColorLength;

        // back pos, used for software bone animation
        float* mGeoPos;
        
        bool mIsBound;
        int mVertexMode;
        int mAttributeFlag;
        uint mPositionHandle;
        uint mTextureHandle;
        uint mNormalHandle;
        uint mJointIndexHandle;
        uint mJointWeightHandle;
        uint mTangentHandle;
        uint mBinoramlHandle;
        uint mColorHandle;

        bool mUseVBO;
        uint* mVBOs;
    };

    typedef SmartPtr<VertexBuffer> VertexBufferPtr;
}

#endif
