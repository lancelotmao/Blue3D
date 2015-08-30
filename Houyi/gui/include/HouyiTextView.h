//
//  HouyiTextView.h
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiTextView_h
#define Houyi_HouyiTextView_h

#include "HouyiImageView.h"

namespace Houyi
{
    class TextView : public ImageView
    {
    public:
        TextView(string text);
        
        void setText(string text);
        HRect getSize();
        void setPosition(Vector2 pos, int halign = EHAlignLeft, int valign = EVAlignTop);
        
        inline void setTextColor(int color)
        {
            if(mTextColor != color)
            {
                mTextColor = color;
                invalidate();
            }
        }
        
        inline void setTextSize(float size)
        {
            if (mTextSize != size)
            {
                mTextSize = size;
                invalidate();
            }
        }
        
        virtual void invalidate();

    protected:
        string mText;
        int mTextColor;
        float mTextSize;
        CanvasPtr mCanvas;
    };
}

#endif
