#include "BlueAdapter.h"

#ifdef __APPLE__
#import "DataManager.h"
#import "FileItem.h"
#endif

int BlueAdapter::getSize()
{
#ifdef ANDROID
    lockJNI();
    gJVM->AttachCurrentThread(&gJNIEnv, NULL);
    jclass cls = gJNIEnv->FindClass("com/dwtech/android/data/DataManager");
    jmethodID mid = gJNIEnv->GetStaticMethodID(cls, "getSize", "()I");
    int size = gJNIEnv->CallStaticIntMethod(cls, mid);
    gJNIEnv->DeleteLocalRef(cls);
    unlockJNI();
    return size;
#elif __APPLE__
    int cnt = [DataManager getInstance].itemCount;
    return cnt;
#endif
    return 0;
}

View* BlueAdapter::getView(int index, View* reusableView)
{
    if (getSize() <= 0)
    {
        return 0;
    }
    
    if (!mDefThumb)
    {
        mDefThumb = ImageFactory::createImage("default_thumb.png");
    }

    if (!mImgFocus)
    {
        mImgFocus = ImageFactory::createImage("focus.png");
    }

    int focus = getFocus();

    ImagePtr img;
#ifdef ANDROID
//    LOGD("android getView start");
    lockJNI();
    gJVM->AttachCurrentThread(&gJNIEnv, NULL);
    jclass cls = gJNIEnv->FindClass("com/dwtech/android/data/DataManager");
    jmethodID mid = gJNIEnv->GetStaticMethodID(cls, "getFilmstripThumbnail", "(ILandroid/content/Context;)Landroid/graphics/Bitmap;");
    jobject bitmap = gJNIEnv->CallStaticObjectMethod(cls, mid, index, gContext);
    gJNIEnv->DeleteLocalRef(cls);
    unlockJNI();
    if (bitmap)
    {
        img = ImageFactory::createImage(bitmap);
    }
//    LOGD("android getView end");
#elif __APPLE__
    DataManager* dataMan = [DataManager getInstance];
    if (index < [dataMan.items count])
    {
        FileItem* item = [dataMan.items objectAtIndex:index];
        if (item.thumbnail)
        {
            img = ImageFactory::createImage(item.thumbnail);
        }
    }
#endif
    
    if (!img || img->getWidth() ==0)
    {
        img = mDefThumb;
    }
    
    ImageView* iv = reusableView ? (ImageView*)reusableView : HouyiNew ImageView();
    if (img->getWidth() != img->getHeight())
    {
        int len = min(img->getWidth(), img->getHeight());
        ImagePtr cropped = ImageFactory::createImage(len, len);
        CanvasPtr c = CanvasFactory::getCanvas(cropped);
        c->drawImageCropCenter(img);
        img = cropped;
    }

    if (focus == index)
    {
        ImagePtr combined = ImageFactory::createImage(img->getWidth(), img->getHeight());
        CanvasPtr c = CanvasFactory::getCanvas(combined);
        HRect r(0, 0, img->getWidth(), img->getHeight());
        c->drawImage(img, r, r);
        c->drawImage(mImgFocus, HRect(0, 0, mImgFocus->getWidth(), mImgFocus->getHeight()), r);
        img = combined;
    }
    iv->setImage(img);
    float w = mViewHeight;
    iv->setBound(HRect(index * w, 0, w, mViewHeight), View::EHAlignLeft, View::EVAlignBottom);
    return iv;
}

int BlueAdapter::getFocus()
{
    int focus = 0;
#ifdef ANDROID
    lockJNI();
    gJVM->AttachCurrentThread(&gJNIEnv, NULL);
    jclass cls = gJNIEnv->FindClass("com/dwtech/android/data/DataManager");
    jmethodID midGetFocus = gJNIEnv->GetStaticMethodID(cls, "getFocus", "()I");
    focus = gJNIEnv->CallStaticIntMethod(cls, midGetFocus);
    gJNIEnv->DeleteLocalRef(cls);
    unlockJNI();
#elif __APPLE__
    DataManager* dataMan = [DataManager getInstance];
    focus = dataMan.focus;
#endif
    return focus;
}

void BlueAdapter::onItemClicked(View* view, int index)
{
    LOGD("ListView Item Clicked: %d", index);
    setFocusIndex(index);
#ifdef ANDROID
    lockJNI();
    gJVM->AttachCurrentThread(&gJNIEnv, NULL);
    jclass cls = gJNIEnv->FindClass("com/dwtech/android/data/DataManager");
    jmethodID mid = gJNIEnv->GetStaticMethodID(cls, "setFocus", "(Landroid/content/Context;I)V");
    gJNIEnv->CallStaticVoidMethod(cls, mid, gContext, index);
    gJNIEnv->DeleteLocalRef(cls);
    unlockJNI();
#elif __APPLE__
    DataManager* dataMan = [DataManager getInstance];
    [dataMan changeFocus:index];
#endif

    notifyDataChange();
}
