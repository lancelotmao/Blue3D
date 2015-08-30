#ifndef __Houyi__HouyiiOSImage__
#define __Houyi__HouyiiOSImage__

#include <UIKit/UIImage.h>

#include <string>

#include "HouyiImage.h"
#include "RefCount.h"
#include "SmartPtr.h"

using namespace std;

namespace Houyi
{
    class iOSImage : public RefCountBase, public Image
    {
    public:
        iOSImage(UIImage* bitmap);
        iOSImage(string fileName);
        iOSImage(const int width, const int height);
        virtual ~iOSImage();

        virtual int getWidth() const;
        virtual int getHeight() const;
        virtual void rotate(float degree);

        virtual void lock();
        virtual void unlock();

        virtual void* getData() const;
        
        inline UIImage* getUIImage()
        {
            return mBitmap;
        }
        
        inline void setUIImage(UIImage* image)
        {
            mBitmap = image;
        }

    private:
        UIImage* mBitmap;
        CFDataRef mCopiedData;
    };

    typedef SmartPtr<iOSImage> ImagePtr;
}
#endif
