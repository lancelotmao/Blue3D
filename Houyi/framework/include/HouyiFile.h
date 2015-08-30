//
//  HouyiFile.h
//  Houyi
//
//  Created by Lance Mao on 5/6/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiFile_h
#define Houyi_HouyiFile_h

#include "HouyiObject.h"
#include "HouyiStringUtils.h"
#ifdef ANDROID
#include <android/asset_manager.h>
#endif

namespace Houyi
{
    class File : public Object
    {
    public:
        File(const char * filename, const char * mode);
        ~File();
        
        bool Open();
        void Close();
        
        int Seek(long int offset, int origin);
        long int Tell();
        void Rewind();
        size_t Read(void * ptr, size_t size, size_t count);
        
    private:
        const char* mFileName;
        const char* mMode;
        FILE* mStream;
#ifdef ANDROID
        AAsset* mAasset;
#endif
    };
}

#endif
