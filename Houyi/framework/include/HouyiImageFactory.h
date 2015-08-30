#ifndef __Houyi__HouyiImageFactory__
#define __Houyi__HouyiImageFactory__

#include <string>
#include "Platform.h"

using namespace std;

namespace Houyi
{
    class ImageFactory
    {
    public:
        static ImagePtr createImage(const string& fileName);
        static ImagePtr createImage(const char* fileName);
        static ImagePtr createImage(const int width, const int height);

#ifdef ANDROID
        static ImagePtr createImage(jobject bitmap);
#elif __APPLE__
        static ImagePtr createImage(UIImage* image);
#endif

    private:
        ImageFactory(){}
    };
}
#endif
