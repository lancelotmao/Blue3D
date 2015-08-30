//
//  HouyiCanvas.h
//  Houyi
//
//  Created by Lance Mao on 19/11/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiCanvas_h
#define Houyi_HouyiCanvas_h

#include <string>

#include "HRect.h"

#ifdef ANDROID
#include "HouyiAndroidImage.h"
#elif __APPLE__
#include "HouyiiOSImage.h"
#endif

using namespace std;

namespace Houyi
{
    class Canvas
    {
    public:
        enum
        {
            EHAlignLeft, EHAlignCenter, EHAlignRight
        };
        
        enum
        {
            EVAlignTop, EVAlignCenter, EVAlignBottom
        };
        
        enum WrapMode
        {
            EWrapL2RH, EWrapL2RV, EWrapR2LH, EWrapR2LV
        };

        Canvas(ImagePtr image);
        virtual ~Canvas();
        
        // abstract functions
        virtual void drawColor(int color) = 0;
        virtual void drawLine(float x1, float y1, float x2, float y2) = 0;
        virtual void drawRect(const HRect& rect) = 0;
        virtual void fillRect(const HRect& rect) = 0;
        virtual void drawString(const string& text, int x, int y, int hflag = EHAlignLeft, int vflag = EVAlignBottom) = 0;
        virtual void drawImage(const ImagePtr image, const HRect& src, const HRect& dst) = 0;
        
        // with implementation
        virtual void drawImageFill(const ImagePtr image);
        virtual void drawImageCropCenter(const ImagePtr image);
        virtual void drawImageFitCenter(const ImagePtr image);
        virtual void setColor(int color);
        virtual void setFontSize(int size);
        virtual void setLineWidth(float size);
        virtual void drawStringWrapped(const string& text, int borderX, int borderY, WrapMode wrapMode = EWrapL2RH);
        virtual void setRotate(float radian);
        virtual void setScaleX(float sx);
        virtual void setScaleY(float sy);

    protected:
        // derived must implement these to get things to work
        virtual void setPaintFontSize() = 0;
        virtual float getStringWidth(const string& str) = 0;

    protected:
        ImagePtr mImage;
        int mColor;
        int mFontSize;
        float mLineWidth;
        float mRotation;
        float mScaleX;
        float mScaleY;
    };
}

#endif
