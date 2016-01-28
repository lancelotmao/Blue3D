#ifndef __Houyi__HouyiTexture__
#define __Houyi__HouyiTexture__

#include "HouyiLog.h"
#include "HouyiObject.h"

namespace Houyi
{
    class TextureManager;
    
    class TextureConfig
    {
    public:
        
        const static TextureConfig DEFAULT;
        
        TextureConfig() : mUploadImmediately(false), mCreateNewImageForNPOT(true)
        {}
        
        bool mUploadImmediately;
        bool mCreateNewImageForNPOT;
    };
    
    class Texture: public Object
    {
    public:
        enum
        {
            EUploadFull,
            EInvalid,
            EValid
        };
        
        static const int WRAP_REPEAT = 0;
        static const int WRAP_CLAMP = 1;

        Texture();

        Texture(ImagePtr image);

        virtual ~Texture();

        virtual void setImage(ImagePtr image, TextureManager* tm);

        inline ImagePtr getImage() const
        {
            return mImage;
        }

        virtual int getTextureId() const
        {
            return mTextureId;
        }

        virtual void setTextureId(int id)
        {
            mTextureId = id;
        }

        virtual float getUScaleFactor() const
        {
            return mUScaleFactor;
        }

        virtual void setUScaleFactor(float factor)
        {
            mUScaleFactor = factor;
        }

        virtual float getVScaleFactor() const
        {
            return mVScaleFactor;
        }

        virtual void setVScaleFactor(float factor)
        {
            mVScaleFactor = factor;
        }

        virtual float getUScroll() const
        {
            return mUScroll;
        }

        virtual void setUScroll(float factor)
        {
            mUScroll = factor;
        }

        virtual float getVScroll() const
        {
            return mVScroll;
        }

        virtual void setVScroll(float factor)
        {
            mVScroll = factor;
        }

        virtual int getWidth() const
        {
            return mWidth;
        }

        virtual int getHeight() const
        {
            return mHeight;
        }

        virtual void setWrapMode(int s, int t)
        {
            mSWrap = s;
            mTWrap = t;
        }

        virtual int getSWrap() const
        {
            return mSWrap;
        }

        virtual int getTWrap() const
        {
            return mTWrap;
        }

        virtual void uploadTexture()
        {
        }

        virtual void allocTexMemory()
        {
        }

        virtual void uploadTexSubImage2D()
        {
        }

        virtual void uploadTexSubImage2D(ImagePtr bitmap, int x, int y, int width, int height)
        {
        }

        virtual void uploadTexSubImage2D(void* data, int x, int y, int width, int height)
        {
        }
        
        virtual int getState() const
        {
            return mState;
        }
        
        virtual void setState(int state)
        {
            mState = state;
        }
        
        virtual void invalidate();
        virtual void invalidate(HRect dirty);

        virtual float getFitCenterOffsetU() const
        {
        	return mFitCenterOffsetU;
        }

        virtual float getFitCenterOffsetV() const
        {
            return mFitCenterOffsetV;
        }

        virtual void setFitCenterOffsetU(float u)
        {
            mFitCenterOffsetU = u;
        }

        virtual void setFitCenterOffsetV(float v)
        {
            mFitCenterOffsetV = v;
        }
        
        inline const HRect& getDirtyRect() const
        {
            return mDirty;
        }

        inline void setType(int type)
        {
            mType = type;
        }

        inline int getType() const
        {
            return mType;
        }

    protected:
        ImagePtr mImage;
        int mWidth;
        int mHeight;
        int mTextureId;
        float mUScaleFactor;
        float mVScaleFactor;
        float mUScroll;
        float mVScroll;
        int mSWrap;
        int mTWrap;
        int mState;
        float mFitCenterOffsetU;
        float mFitCenterOffsetV;
        HRect mDirty;
        int mType; // 2D or OES
    };
}

#endif
