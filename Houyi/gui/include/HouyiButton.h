//
//  HouyiButton.h
//  Houyi
//
//  Created by Lance Mao on 12/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiButton_h
#define Houyi_HouyiButton_h

#include "HouyiImageView.h"

namespace Houyi
{
    class Button : public ImageView
    {
    public:
        Button();
        virtual ~Button();
        
        virtual bool onTouch(MotionEvent e);
        
        inline void setNormalImage(ImagePtr img)
        {
            mImageNormal = img;
            setImage(mImageNormal);
        }
        
        inline void setPressedImage(ImagePtr img)
        {
            mImagePressed = img;
        }
        
        virtual void cancel();

    protected:
        ImagePtr mImageNormal;
        ImagePtr mImagePressed;
    };
}
#endif
