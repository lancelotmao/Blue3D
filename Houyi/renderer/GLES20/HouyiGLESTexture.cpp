#include "HouyiGLESTexture.h"
#include "Platform.h"
#include "HouyiUtil.h"
#include "HouyiGLUtils.h"
#include "HouyiRoot.h"
#include "HouyiImageFactory.h"
#include "HouyiCanvasFactory.h"
#include "HouyiTextureManager.h"

namespace Houyi
{
	GLESTexture::GLESTexture(ImagePtr image, TextureConfig config) : Texture(image), mConfig(config)
	{
	    mType = GL_TEXTURE_2D;
		ensurePOT();

		if (config.mUploadImmediately)
		{
			uploadTexture();
		}
	}

	GLESTexture::~GLESTexture()
	{
//		if (!Root::getInstance()->mAutoFinalize)
//		{
//			glDeleteTextures(1, mTextureIds);
//			checkGlError("glDeleteTextures");
//		}
	}

	void GLESTexture::setImage(ImagePtr image, TextureManager* tm)
	{
        if (image == mImage)
        {
            return;
        }
        
        int curWidth = mWidth;
        int curHeight = mHeight;
        
	    Texture::setImage(image, tm);
        
        ensurePOT();
        
        // only upload full after POT check
        if (curWidth != mWidth || curHeight != mHeight)
        {
            mState = EUploadFull;
        }
        else if (mState != EUploadFull)
        {
            mState = EInvalid;
        }
        mDirty = HRect(0, 0, mWidth, mHeight);
        tm->addTextureToUploadQueue(this);
	}

	void GLESTexture::setWrapMode(int s, int t)
	{
        glGenerate();
	    Texture::setWrapMode(s, t);
	    glBindTexture(GL_TEXTURE_2D, mTextureId);
	    if (s == Texture::WRAP_REPEAT)
	    {
	        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    }
	    else if (s == Texture::WRAP_CLAMP)
	    {
	        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    }

	    if (t == Texture::WRAP_REPEAT)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else if (t == Texture::WRAP_CLAMP)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
	}

	void GLESTexture::uploadTexture()
	{
        glGenerate();
	    glBindTexture(GL_TEXTURE_2D, mTextureId);
        checkGlError("glBindTexture");

        allocTexMemory();

		mImage->lock();
		if (mImage->getData() == 0)
		{
			LOGE("SkBitmap getPixels returns NULL\n");
		}
		else
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mImage->getWidth(),
					mImage->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, mImage->getData());

			checkGlError("glTexSubImage2D");
		}
		mImage->unlock();
	}

	void GLESTexture::allocTexMemory()
	{
        glGenerate();
	    glBindTexture(GL_TEXTURE_2D, mTextureId);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
                            0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        checkGlError("allocTexMemory");
	}

	void GLESTexture::uploadTexSubImage2D()
	{
	    if (mImage)
	    {
	        uploadTexSubImage2D(mImage, 0, 0, mImage->getWidth(), mImage->getHeight());
	    }
	}

    void GLESTexture::uploadTexSubImage2D(ImagePtr bitmap, int x, int y, int width, int height)
    {
        if (!bitmap)
        {
            LOGE("uploadTexSubImage2D. NULL bitmap\n");
            return;
        }

        bitmap->lock();
        uploadTexSubImage2D(bitmap->getData(), x, y, width, height);
        bitmap->unlock();
    }

    void GLESTexture::uploadTexSubImage2D(void* data, int x, int y, int width, int height)
    {
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        checkGlError("uploadTexSubImage2D");
    }

    void GLESTexture::ensurePOT()
    {
        // since we are GL texture, ensure POT
        int width = Util::nextPowerOf2(mWidth);
        int height = Util::nextPowerOf2(mHeight);
        if (mImage && width >= 0 && height >= 0 && (width != mWidth || height != mHeight))
        {
        	LOGW("NPOT texture not supported, use next POT. src = [%d,%d], dst = [%d,%d]\n",
        			mWidth, mHeight, width, height);
            if (mConfig.mCreateNewImageForNPOT)
            {
                ImagePtr img = ImageFactory::createImage(width, height);
                CanvasPtr c = CanvasFactory::getCanvas(img);
                c->drawImage(mImage, HRect(0, 0, mWidth, mHeight), HRect(0, 0, width, height));
                mImage = img;
            }
            mUScaleFactor = mImage->getWidth() / (float)width;
            mVScaleFactor = mImage->getHeight() / (float)height;
            mWidth = width;
            mHeight = height;
        }
    }
    
    void GLESTexture::glGenerate()
    {
        if (mTextureId <= 0)
        {
            glGenTextures(1, mTextureIds);
            checkGlError("glGenTextures");
            mTextureId = mTextureIds[0]; 
        }
    }
}
