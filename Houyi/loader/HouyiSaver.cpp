//
//  HouyiSaver.cpp
//  Houyi
//
//  Created by Lance Mao on 5/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiSaver.h"
#include "HouyiDefaultSaver.h"
#include "HouyiStringUtils.h"
#include "HouyiLog.h"

namespace Houyi
{
    Saver::Saver()
    {
        
    }
    
    Saver::~Saver()
    {
    }
    
    string Saver::getDefaultAppWritablePath(const string& filePath)
    {
        string fn = filePath;
        StringUtils::replace(fn, "/", "%_%");
#ifdef ANDROID
        return fn;
#elif defined __APPLE__
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        string strDoc = documentsDirectory.UTF8String;
        return strDoc.append("/").append(fn);
#endif
    }
    
    Saver* Saver::getSaver(const string& filePath)
    {
        Saver* saver = 0;
        
        // for now only support .houyi file which is the default file for houyi engine
        saver = HouyiNew DefaultSaver();
        
        saver->mFilePath = filePath;
        return saver;
    }
    
    bool Saver::save(Scene* scene)
    {
        bool ret = true;
        LOGD("start saving file: %s\n", mFilePath.c_str());
        mFileStream.open (mFilePath.c_str(), ios_base::binary);
        if (mFileStream.is_open())
        {
            saveScene(scene);
        }
        else
        {
            // most probably is lack of permission
            LOGE("cannot open file: %s\n", mFilePath.c_str());
            ret = false;
        }
        mFileStream.close();
        LOGD("done saving file: %s\n", mFilePath.c_str());

        HouyiDelete(this);
        return ret;
    }
}
