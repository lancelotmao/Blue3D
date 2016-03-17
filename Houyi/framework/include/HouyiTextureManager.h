//
//  HouyiTextureManager.h
//  Houyi
//
//  Created by Lance Mao on 21/11/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef __Houyi__HouyiTextureManager__
#define __Houyi__HouyiTextureManager__

#include <pthread.h>
#include <vector>
#include <deque>
#include <map>
#include "HouyiAllocator.h"
#include "HouyiTexture.h"

using namespace std;

namespace Houyi
{
    class Scene;
    
    class TextureManager : public Allocator
    {
    public:
//        static TextureManager* getInstance();
        TextureManager();
        virtual ~TextureManager();
        
        Texture* createTexture(Scene* scene, const string& imageName, TextureConfig config = TextureConfig::DEFAULT);
        Texture* createTexture(ImagePtr image, TextureConfig config = TextureConfig::DEFAULT);
        Texture* createCubeMapTexture(ImagePtr images[6]);
        void removeTexture(Scene* scene, Texture* texture);
        void removeAllTexture();
        void addTextureToUploadQueue(Texture* texture);
        
        void upload();
        
        inline int getUploadQuota()
        {
            return mUploadQuota;
        }
        
        inline void setUploadQuota(int q)
        {
            mUploadQuota = q;
        }

        inline bool hasPendingTexture()
        {
            return mUploadQueue.size() > 0;
        }

    private:
        deque<Texture*> mUploadQueue;
        int mUploadQuota;
        pthread_mutex_t mMutex;
        map<string, Texture*> mTextureMap;
    };
}

#endif /* defined(__Houyi__HouyiTextureManager__) */
