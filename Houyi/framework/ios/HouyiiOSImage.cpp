#include "Houyi.h"

namespace Houyi
{
    iOSImage::iOSImage(UIImage* bitmap) : mBitmap(bitmap), mCopiedData(0)
    {

    }

    iOSImage::iOSImage(string fileName) : mBitmap(NULL), mCopiedData(0)
    {
        NSString* path = nil;
        if (fileName.find('/') != string::npos)
        {
            path = [NSString stringWithCString:fileName.c_str() encoding:NSUTF8StringEncoding];
        }
        else
        {
            int pos = fileName.find('.');
            string name;
            string type;
            if (pos != string::npos)
            {
                name = fileName.substr(0, pos);
                type = fileName.substr(pos + 1, fileName.length());
            }
            NSString* nsFileName = [NSString stringWithCString:name.c_str() encoding:NSASCIIStringEncoding];
            NSString* nsFileType = [NSString stringWithCString:type.c_str() encoding:NSASCIIStringEncoding];
            path = [[NSBundle mainBundle] pathForResource:nsFileName ofType:nsFileType];
        }
        
        mBitmap = [UIImage imageWithContentsOfFile:path];
        if (!mBitmap)
        {
            LOGD("cannot create bitmap = %s\n", fileName.c_str());
        }
    }

    iOSImage::iOSImage(const int width, const int height) : mBitmap(NULL), mCopiedData(0)
    {
        CGRect rect = CGRectMake(0.0f, 0.0f, width, height);
        UIGraphicsBeginImageContext(rect.size);
        mBitmap = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
    }

    iOSImage::~iOSImage()
    {
    }

    int iOSImage::getWidth() const
    {
        if (mBitmap)
        {
            return mBitmap.size.width * mBitmap.scale;
        }
        return 0;
    }

    int iOSImage::getHeight() const
    {
        if (mBitmap)
        {
            return mBitmap.size.height * mBitmap.scale;
        }
        return 0;
    }
    
    void iOSImage::rotate(float degree)
    {
        // TODO software rotate image by any degree
    }

    void iOSImage::lock()
    {
        if (mCopiedData)
        {
            LOGE("iOSImage_lockPixels. pixel locked\n");
            return;
        }
        
        if (!mBitmap)
        {
            LOGE("iOSImage_lockPixels. NULL bitmap\n");
            return;
        }

        CGImageRef image = [mBitmap CGImage];
        mCopiedData = CGDataProviderCopyData(CGImageGetDataProvider(image));
    }

    void iOSImage::unlock()
    {
        if (mCopiedData)
        {
            CFRelease(mCopiedData);
        }
        mCopiedData = 0;
    }

    void* iOSImage::getData() const
    {
        if (!mCopiedData)
        {
            LOGE("iOSImage_lockPixels() failed! Data is NULL\n");
            return 0;
        }
        
        void* data = (void*)CFDataGetBytePtr(mCopiedData);
        if (!data)
        {
            LOGE("iOSImage_lockPixels() failed!\n");
        }
        return data;
    }
}
