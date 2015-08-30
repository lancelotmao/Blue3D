#ifndef __Houyi__HouyiAndroidImage__
#define __Houyi__HouyiAndroidImage__

#include <jni.h>

#include "HouyiImage.h"
#include "RefCount.h"
#include "SmartPtr.h"

namespace Houyi
{
    class AndroidImage : public RefCountBase, public Image
    {
    public:
        AndroidImage(jobject bitmap);
        AndroidImage(JNIEnv* env, const char* fileName);
        AndroidImage(JNIEnv* env, const int width, const int height);
        virtual ~AndroidImage();

        virtual int getWidth() const;
        virtual int getHeight() const;

        virtual void lock();
        virtual void unlock();

        virtual void* getData() const;

        inline jobject getBitmap()
        {
            return mBitmap;
        }

    private:
        jobject mBitmap;
        void* mLockedPixel;
        bool mOwn;
    };

    typedef SmartPtr<AndroidImage> ImagePtr;
}
#endif
