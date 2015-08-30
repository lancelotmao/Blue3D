/*
 * HouyiAndroidCanvas.h
 *
 *  Created on: Nov 20, 2013
 *      Author: lancemao
 */

#ifndef HOUYIANDROIDCANVAS_H_
#define HOUYIANDROIDCANVAS_H_

#include <jni.h>

#include "HouyiCanvas.h"
#include "RefCount.h"
#include "SmartPtr.h"

namespace Houyi
{
    class AndroidCanvas : public RefCountBase, public Canvas
    {
    public:
        AndroidCanvas(ImagePtr image);

        virtual ~AndroidCanvas(){}

        virtual void drawColor(int color);
        virtual void drawLine(float x1, float y1, float x2, float y2);
        virtual void drawRect(const HRect& rect);
        virtual void fillRect(const HRect& rect);
        virtual void drawString(const string& text, int x, int y, int hflag = EHAlignLeft, int vflag = EVAlignBottom);
        virtual void drawImage(const ImagePtr image, const HRect& src, const HRect& dst);

    protected:
        virtual void setPaintFontSize();
        virtual float getStringWidth(const string& str);

    private:
        JNIEnv* mEnv;
        jobject mJCanvas;
        jobject mJPaint;
        jclass mClsCanvas;
        jclass mClsPaint;
        jclass mClsRect;
    };

    typedef SmartPtr<AndroidCanvas> CanvasPtr;
}

#endif /* HOUYIANDROIDCANVAS_H_ */
