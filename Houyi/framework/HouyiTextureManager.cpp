//
//  HouyiTextureManager.cpp
//  Houyi
//
//  Created by Lance Mao on 21/11/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "HouyiTextureManager.h"
#include "HouyiRoot.h"
#include "HouyiGLESTexture.h"
#include "HouyiGLESCubeMapTexture.h"
#include "HouyiImageFactory.h"
#include "HouyiCanvasFactory.h"
#include "HouyiScene.h"

namespace Houyi
{
    TextureManager* TextureManager::mIns = 0;
    
    TextureManager::TextureManager() : mUploadQuota(1)
    {
        pthread_mutex_init(&mMutex, 0);
    }
    
//    TextureManager* TextureManager::getInstance()
//    {
//        if (!mIns)
//        {
//            mIns = HouyiNew TextureManager();
//            pthread_mutex_init(&mIns->mMutex, 0);
//        }
//        return mIns;
//    }
    
    TextureManager::~TextureManager()
    {
        removeAllTexture();

        pthread_mutex_destroy(&mMutex);

        mIns = 0;
    }
    
    Texture* TextureManager::createTexture(Scene* scene, const string& imageName, TextureConfig config)
    {
        string sceneName = scene->getName();
        string key = sceneName + imageName;
        map<string, Texture*>::iterator it = mTextureMap.find(key);
        if (it != mTextureMap.end())
        {
            return it->second;
        }

        ImagePtr image = ImageFactory::createImage(imageName);
        if (image && image->getWidth() > 0 && image->getHeight() > 0)
        {
            Texture* texture = createTexture(image, config);
            if (texture)
            {
                texture->setName(imageName);
                mTextureMap.insert(pair<string, Texture*>(key, texture));
                scene->addTexture(texture);
            }
            return texture;
        }
        else
        {
            LOGE("TextureManager::createTexture. create image failed. path = %s\n", imageName.c_str());
            return NULL;
        }
    }

    Texture* TextureManager::createTexture(ImagePtr image, TextureConfig config)
    {
    	if (!image)
    	{
    		LOGE("TextureManager::createTexture. NULL image\n");
    		return NULL;
    	}

        Texture* t = HouyiNew GLESTexture(image, config);
        if (t)
        {
            pthread_mutex_lock(&mMutex);
            mUploadQueue.push_back(t);
            pthread_mutex_unlock(&mMutex);

//            Root* root = Root::getInstance();
//            root->requestRender();
        }
        else
        {
            LOGE("TextureManager::createTexture Texture NULL\n");
        }
        return t;
    }
    
    Texture* TextureManager::createCubeMapTexture(ImagePtr images[6])
    {
        Texture* t = HouyiNew GLESCubeMapTexture(images, false);
        if (t)
        {
            pthread_mutex_lock(&mMutex);
            mUploadQueue.push_back(t);
            pthread_mutex_unlock(&mMutex);

//            Root* root = Root::getInstance();
//            root->requestRender();
        }
        else
        {
            LOGE("TextureManager::createCubeMapTexture Texture NULL\n");
        }
        return t;
    }
    
    void TextureManager::removeTexture(Scene* scene, Texture* texture)
    {
        if (!texture || !scene) return;

        pthread_mutex_lock(&mMutex);

        for( deque<Texture*>::iterator i(mUploadQueue.begin()), end(mUploadQueue.end()); i != end; ++i )
        {
            Texture* t = *i;
            if (t == texture)
            {
                mUploadQueue.erase(i);
                break;
            }
        }
        
        string name = scene->getName() + texture->getName();
        map<string, Texture*>::iterator it = mTextureMap.find(name);
        if (it != mTextureMap.end())
        {
            mTextureMap.erase(it);
        }

        pthread_mutex_unlock(&mMutex);
    }
    
    void TextureManager::removeAllTexture()
    {
        pthread_mutex_lock(&mMutex);
        mUploadQueue.clear();
        mTextureMap.clear();
        pthread_mutex_unlock(&mMutex);
    }

    void TextureManager::addTextureToUploadQueue(Texture* texture)
    {
        pthread_mutex_lock(&mMutex);
        bool alreadyAddToQueue = false;
        for (int i = 0;i < mUploadQueue.size();++i)
        {
        	if (mUploadQueue[i] == texture)
        	{
        		alreadyAddToQueue = true;
        		break;
        	}
        }
        if (!alreadyAddToQueue)
        {
        	mUploadQueue.push_back(texture);
        }
        pthread_mutex_unlock(&mMutex);
    }

    void TextureManager::upload()
    {
        pthread_mutex_lock(&mMutex);

        if (mUploadQueue.size() > 0)
        {
            LOGI("TextureManager::upload. Queue = %zd\n", mUploadQueue.size());

            int q = 0;
            do
            {
                Texture* t = mUploadQueue.front();
                if (t && t->getState() != Texture::EValid)
                {
                    if (t->getState() == Texture::EUploadFull)
                    {
//                        LOGI("TextureManager::upload full\n");
                        t->allocTexMemory();
                    }

                    const HRect& dirty = t->getDirtyRect();
                    ImagePtr img = t->getImage();
                    if (!img)
                    {
                        // cube map
                        t->uploadTexture();
                    }
                    else if (t->getState() == Texture::EUploadFull || (dirty.mLeft == 0 && dirty.mTop == 0
                        && dirty.mWidth == img->getWidth() && dirty.mHeight == img->getHeight()))
                    {
//                        LOGD("dirty region same as source image\n");
                        t->uploadTexSubImage2D(t->getImage(), 0, 0, img->getWidth(), img->getHeight());
                    }
                    else if ((int)dirty.mWidth > 0 && (int)dirty.mHeight > 0)
                    {
//                        LOGD("l = %f, t = %f, w = %f, h = %f\n", dirty.mLeft, dirty.mTop, dirty.mWidth, dirty.mHeight);
                        ImagePtr subImg = ImageFactory::createImage(dirty.mWidth, dirty.mHeight);
                        CanvasPtr c = CanvasFactory::getCanvas(subImg);
                        c->drawImage(img, dirty, HRect(0, 0, subImg->getWidth(), subImg->getHeight()));
                        t->uploadTexSubImage2D(subImg, (int)dirty.mLeft, (int)dirty.mTop, (int)dirty.mWidth, (int)dirty.mHeight);
                    }
                    else
                    {
                        LOGW("should not come here l = %f, t = %f, w = %f, h = %f\n", dirty.mLeft, dirty.mTop, dirty.mWidth, dirty.mHeight);
                    }
                    t->setState(Texture::EValid);
                    q++;
//                    Root* root = Root::getInstance();
//                    root->requestRender();
                }
                mUploadQueue.erase(mUploadQueue.begin());
            } while (q < mUploadQuota && mUploadQueue.size() > 0);
        }

        pthread_mutex_unlock(&mMutex);
    }
}
