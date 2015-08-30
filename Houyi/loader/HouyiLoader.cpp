//
//  HLoader.cpp
//  houyi
//
//  Created by Lance Mao on 2/10/11.
//  Copyright (c) 2011 Dong wen. All rights reserved.
//

#include <iostream>
#include "HouyiLoader.h"
#include "HouyiStringUtils.h"
#include "HouyiSceneManager.h"

#include "HouyiDefaultLoader.h"
#include "HouyiColladaLoader.h"
#include "HouyiObjLoader.h"
#include "Houyi3dsLoader.h"
#include "HouyiMapLoader.h"
#include "HouyiStlLoader.h"

namespace Houyi
{
    Loader::Loader() : mScene(0), mIsCanceled(false), mProgress(0), mOption(0)
    {
    }
    
    Loader::Loader(string fp) : mScene(0), mFilePath(fp), mIsCanceled(false), mProgress(0), mOption(0)
    {
        LOGD("Init loader. mFilePath = %s\n", fp.c_str());
    }

    Loader::~Loader()
    {
        
    }

    Loader* Loader::getLoader(string filePath)
    {
        Loader* loader = 0;
        if (StringUtils::hasSuffix(filePath, ".houyi"))
        {
            loader = HouyiNew DefaultLoader();
        }
        // due to Apple will convert dae to binary, we add hae to avoid processing
        else if (StringUtils::hasSuffix(filePath, ".dae") || StringUtils::hasSuffix(filePath, ".hae"))
        {
            loader = HouyiNew collada::ColladaLoader(filePath);
        }
        else if (StringUtils::hasSuffix(filePath, ".tmx"))
        {
            loader = HouyiNew MapLoader();
        }
        else if (StringUtils::hasSuffix(filePath, ".obj"))
        {
            loader = HouyiNew ObjLoader();
        }
        else if (StringUtils::hasSuffix(filePath, ".3ds"))
        {
            loader = HouyiNew Houyi3dsLoader();
        }
        else if (StringUtils::hasSuffix(filePath, ".STL"))
        {
            loader = HouyiNew StlLoader();
        }
        else
        {
            LOGE("unsupported file: %s\n", filePath.c_str());
            return 0;
        }
        loader->mFilePath = filePath;
        return loader;
    }

    Scene* Loader::load(const char* data, int size, int option)
	{
        double startTime = currentTimeMillis();
        LOGD("start loading file: %s\n", mFilePath.c_str());
        mOption = option;
        mProgress = 0;
        mScene = HouyiNew Scene();
        mScene->setName(mFilePath);
        mScene->setUseFPCamera(useFPCamera());
        if (isZup())
            mScene->setZAsUpAxis();
        mScene = loadFromMemory(data, size);
        LOGD("done loading file: %s\n", mFilePath.c_str());
        if (mScene)
        {
            if (!StringUtils::endsWith(mFilePath, ".houyi"))
            {
                // for .houyi file, already optimized
                mScene->prepare(this);
            }
        }
        if (mIsCanceled)
        {
            HouyiDelete(mScene);
            mScene = 0;
        }
        LOGD("done optimizing file (%s). used %f second(s)\n",
                     mFilePath.c_str(), ((currentTimeMillis() - startTime) / 1000));
		return mScene;
	}
}
