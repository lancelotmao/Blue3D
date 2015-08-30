//
//  HouyiFile.cpp
//  Houyi
//
//  Created by Lance Mao on 5/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiFile.h"
#include "HouyiLog.h"

namespace Houyi
{
    File::File(const char * filename, const char * mode) :
    mFileName(filename), mMode(mode), mStream(0)
    {
#ifdef ANDROID
    	mAasset = NULL;
#endif
    }
    
    File::~File()
    {
        Close();
    }
    
    bool File::Open()
    {
        mStream = fopen(mFileName, mMode);
        if (!mStream)
        {
#ifdef ANDROID
        	mAasset = AAssetManager_open(gAssetManager, mFileName, 0);
        	if(mAasset) {
            	return true;
        	} else {
        		LOGE("Cannot open file in assets folder. %s\n", mFileName);
        		return false;
        	}
#elif defined __APPLE__
            // when file is under main bundle
            string fn = StringUtils::removeSuffix(mFileName);
            string suffix = StringUtils::getSuffix(mFileName);
            
            NSString* nsFn = [NSString stringWithCString:fn.c_str() encoding:NSUTF8StringEncoding];
            NSString* nsSuffix = [NSString stringWithCString:suffix.c_str() encoding:NSUTF8StringEncoding];
            NSString * path = [[NSBundle mainBundle] pathForResource: nsFn ofType: nsSuffix];
            mStream = fopen(path.UTF8String, mMode);
#endif
        }
        return mStream != 0;
    }

    void File::Close()
    {
#ifdef ANDROID
    	if(mAasset) {
    		AAsset_close(mAasset);
    		return;
    	}
#endif
        if (mStream)
        {
            fclose(mStream);
        }
    }
    
    int File::Seek(long int offset, int origin)
    {
#ifdef ANDROID
    	if(mAasset)
    		return AAsset_seek(mAasset, offset, origin);
#endif
        return fseek(mStream, offset, origin);
    }
    
    long int File::Tell()
    {
#ifdef ANDROID
    	if(mAasset)
    		return AAsset_getLength(mAasset);
#endif

        return ftell(mStream);
    }
    
    void File::Rewind()
    {
#ifdef ANDROID
    	if(mAasset) {
    		AAsset_seek(mAasset, 0, SEEK_SET);
    		return;
    	}
#endif
        rewind(mStream);
    }
    
    size_t File::Read(void * ptr, size_t size, size_t count)
    {
#ifdef ANDROID
    	if(mAasset)
    		return AAsset_read(mAasset, ptr, count);
#endif
        return fread(ptr, size, count, mStream);
    }
}
