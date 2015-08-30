//
//  HouyiButton.cpp
//  Houyi
//
//  Created by Lance Mao on 12/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiButton.h"

namespace Houyi
{
    Button::Button() : mImageNormal(0), mImagePressed(0)
    {
        mName = "Button";
        mMaterial->setTextureMode(RenderState::EModulate);
        mMaterial->setDiffuseColor(0xffffffff);
    }
    
    Button::~Button()
    {

    }

    bool Button::onTouch(MotionEvent e)
    {
        if (e.mType == MotionEvent::TYPE_DOWN)
        {
            if (mImagePressed)
            {
                setImage(mImagePressed);
            }
            else
            {
                mMaterial->setDiffuseColor(0xffaaaaaa);
            }
            mIsActive = true;
        }
        else if (e.mType == MotionEvent::TYPE_UP)
        {
            cancel();
        }
        return true;
    }

    void Button::cancel()
    {
        setImage(mImageNormal);
        mMaterial->setDiffuseColor(0xffffffff);
    }
}
