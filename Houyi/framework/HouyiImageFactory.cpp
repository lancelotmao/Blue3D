#include "HouyiImageFactory.h"

#include "Platform.h"

namespace Houyi
{
    ImagePtr ImageFactory::createImage(const string& fileName)
    {
        return createImage(fileName.c_str());
    }

    ImagePtr ImageFactory::createImage(const char* fileName)
    {
#ifdef ANDROID
    	ImagePtr img = new AndroidImage(gJNIEnv, fileName);
        return img->getBitmap() ? img : 0;
#elif __APPLE__
        ImagePtr img = new iOSImage(fileName);
        return img->getUIImage() ? img : 0;
#endif
        return 0;
    }

    ImagePtr ImageFactory::createImage(const int width, const int height)
    {
#ifdef ANDROID
        return new AndroidImage(gJNIEnv, width, height);
#elif __APPLE__
        return new iOSImage(width, height);
#endif
        return 0;
    }

#ifdef ANDROID
    ImagePtr ImageFactory::createImage(jobject bitmap)
    {
        return new AndroidImage(bitmap);
    }
#elif __APPLE__
    ImagePtr ImageFactory::createImage(UIImage* image)
    {
        return new iOSImage(image);
    }
#endif
}
