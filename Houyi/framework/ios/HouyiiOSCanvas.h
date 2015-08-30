//
//  HouyiiOSCanvas.h
//  Houyi
//
//  Created by Lance Mao on 19/11/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#ifndef __Houyi__HouyiiOSCanvas__
#define __Houyi__HouyiiOSCanvas__

#include "RefCount.h"
#include "SmartPtr.h"

#include "HouyiCanvas.h"

namespace Houyi
{
    class iOSCanvas : public Canvas, public RefCountBase
    {
    public:
        iOSCanvas(ImagePtr image);
        ~iOSCanvas();

        virtual void drawColor(int color);
        virtual void drawLine(float x1, float y1, float x2, float y2);
        virtual void drawRect(const HRect& rect);
        virtual void fillRect(const HRect& rect);
        virtual void drawString(const string& text, int x, int y, int hflag = EHAlignLeft, int vflag = EVAlignBottom);
        virtual void drawImage(const ImagePtr image, const HRect& src, const HRect& dst);
        
    protected:
        virtual void setPaintFontSize(){};
        virtual float getStringWidth(const string& str);
        
    private:
        unsigned char *mData;
        CGContextRef mContext;
    };
    
    typedef SmartPtr<iOSCanvas> CanvasPtr;
}
#endif /* defined(__Houyi__HouyiiOSCanvas__) */
