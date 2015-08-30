#include "HouyiTexture.h"
#include "HouyiTextureManager.h"

namespace Houyi
{
    const TextureConfig TextureConfig::DEFAULT;
    
    Texture::Texture() : mTextureId(0), mUScaleFactor(1), mVScaleFactor(1),
    mWidth(0), mHeight(0), mUScroll(0), mVScroll(0),
    mSWrap(WRAP_CLAMP), mTWrap(WRAP_CLAMP), mState(EUploadFull)
    {
        mName = "tex";
    }

    Texture::Texture(ImagePtr image) : mImage(image), mTextureId(-1)
    {
        mName = "tex";
        
        if (mImage)
        {
            mWidth = image->getWidth();
            mHeight = image->getHeight();
            mState = EUploadFull;
        }
        else
        {
            LOGE("Trying to create texture from null image\n");
        }
        mUScaleFactor = 1.0f;
        mVScaleFactor = 1.0f;
        mUScroll = 0;
        mVScroll = 0;
        mFitCenterOffsetU = 0;
        mFitCenterOffsetV = 0;
        mDirty = HRect(0, 0, mWidth, mHeight);
    }

    Texture::~Texture()
    {
    }

    void Texture::setImage(ImagePtr image)
    {
        mImage = image;
        mWidth = image->getWidth();
        mHeight = image->getHeight();
    }
    
    void Texture::invalidate()
    {
        if (mImage)
        {
            invalidate(HRect(0, 0, mImage->getWidth(), mImage->getHeight()));
        }
        else
        {
            // cube map
            invalidate(HRect(0, 0, 0, 0));
        }
    }
    
    void Texture::invalidate(HRect dirty)
    {
        mDirty = dirty;
        if (mDirty.mLeft < 0)
        {
            mDirty.mLeft = 0;
        }
        if (mDirty.mTop < 0)
        {
            mDirty.mTop = 0;
        }
        if (mDirty.mLeft + mDirty.mWidth > mWidth)
        {
            mDirty.mWidth = mWidth - mDirty.mLeft;
        }
        if (mDirty.mTop + mDirty.mHeight > mHeight)
        {
            mDirty.mHeight = mHeight - mDirty.mTop;
        }
        
        // mState might be set to EUploadFull already
        if (mTextureId >= 0 && mState != EUploadFull)
        {
            mState = EInvalid;
        }
        else
        {
            mState = EUploadFull;
        }
        TextureManager::getInstance()->addTextureToUploadQueue(this);
    }
}
