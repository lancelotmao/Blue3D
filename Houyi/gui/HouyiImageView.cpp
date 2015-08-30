#include "HouyiImageView.h"
#include "HouyiGeoPlane.h"
#include "HouyiTextureManager.h"
#include "HouyiScene.h"

namespace Houyi
{
    ImageView::ImageView() : mTexture(0), mFitType(EFill)
    {
        GeoPlane* plane = HouyiNew GeoPlane(1, 1, 1, 1, false, false);
        plane->generate(VertexBuffer::ENormal,
                        VertexBuffer::Attribute_Texcoord | GeoPlane::Attribute_FillZ | VertexBuffer::Attribute_Normal ,
                        Geometry::Generate_Triangle_Index);
        mMesh = plane;
        mSelectable = false; // don't use 3D ray collision test, use 2D rect test instead
        mName = "ImageView";
    }

    ImageView::~ImageView()
    {
    }

    void ImageView::setTexture(Texture* texture)
    {
        if (mTexture != texture)
        {
            mTexture = texture;
            if (mMaterial->getTextureCount() == 0)
            {
                mMaterial->addTexture(mTexture);
            }
        }
    }

    Texture* ImageView::getTexture()
    {
        return mTexture;
    }

    void ImageView::attachToScene(Scene* scene)
    {
        View::attachToScene(scene);
        if (mTexture)
        {
            scene->addTexture(mTexture);
        }
    }

    void ImageView::setImage(ImagePtr image)
    {
        if (mTexture)
        {
            mTexture->setImage(image);
        }
        else
        {
            TextureConfig config;
            // for editting, better to keep original image
            config.mCreateNewImageForNPOT = false;
            mTexture = TextureManager::getInstance()->createTexture(image, config);
            if (mMaterial->getTextureCount() == 0)
            {
                mMaterial->addTexture(mTexture);
            }
            if (mAttached)
            {
                mAttachedScene->addTexture(mTexture);
            }
        }
        
        mImage = image;
        setTexcoord();
    }

    ImagePtr ImageView::getImage()
    {
        return mImage;
    }

    void ImageView::setFitType(FitType type)
    {
        mFitType = type;
    }
    
    bool ImageView::onLayout(const HRect& bound, World* world)
    {
        if (!mImage || mImage->getWidth() <= 0 || mImage->getHeight() <= 0)
        {
            return false;
        }

        setTexcoord();

        return false;
    }

    void ImageView::invalidate(HRect dirty)
    {
        if (mTexture)
        {
            mTexture->invalidate(dirty);
        }
        View::invalidate(dirty);
    }

    void ImageView::setTexcoord()
    {
        if (!mImage || mBound.mWidth == 0 || mBound.mHeight == 0)
        {
            return;
        }

        if (mFitType == EFitCenter)
        {
            float w = mBound.mWidth;
            float h = mBound.mHeight;
            float iw = mImage->getWidth();
            float ih = mImage->getHeight();
            if (w / h > iw / ih)
            {
                mTexture->setFitCenterOffsetU((1 - iw / ih * h / w) / 2);
                mTexture->setFitCenterOffsetV(0);
            }
            else
            {
                mTexture->setFitCenterOffsetU(0);
                mTexture->setFitCenterOffsetV((1 - ih / iw * w / h) / 2);
            }
        }
    }
}
