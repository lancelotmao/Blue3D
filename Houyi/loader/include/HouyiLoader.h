//
//  HLoader.h
//  houyi
//
//  Created by Lance Mao on 14/08/2011.
//  Copyright 2011 Dong wen. All rights reserved.
//

#ifndef HLOADER_H
#define HLOADER_H

#include <iostream>
#include "HouyiObject.h"
#include "HouyiConst.h"

using namespace std;

namespace Houyi {
    class Scene;
    class Loader : public Object
    {
    protected:
        Scene* mScene;
        string mFilePath;
        bool mIsCanceled;
        float mProgress;
        int mOption;
        
    protected:
        Loader();
        Loader(const string fp);
        
        virtual Scene* loadFromMemory(const char* data, int size) = 0;
        
    public:
        virtual ~Loader();

        static Loader* getLoader(const string filePath);
        Scene* load(const char* data, int size, int option = 0);

        inline float getProgress()
        {
            return mProgress;
        }
        
        inline void notifiyProgress(float progress)
        {
            mProgress = progress;
        }

        inline void cancel()
        {
            mIsCanceled = true;
        }
        
        inline bool isCanceled()
        {
            return mIsCanceled;
        }
        
        inline bool mergeMesh()
        {
            return (mOption & Const::LO_MERGE_MESH) != 0;
        }
        
        inline bool useFPCamera()
        {
            return (mOption & Const::LO_USE_FP) != 0;
        }
        
        inline bool isZup()
        {
            return (mOption & Const::LO_Z_UP) != 0;
        }
        
        inline bool loadAABB()
        {
            return (mOption & Const::LO_AABB) != 0;
        }
    };
}

#endif
