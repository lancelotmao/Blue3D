//
//  HouyiTextView.cpp
//  Houyi
//
//  Created by Lance Mao on 13/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#include "HouyiTextView.h"
#include "HouyiImageFactory.h"
#include "HouyiCanvasFactory.h"

namespace Houyi
{
    TextView::TextView(string text) : mTextColor(0xffffffff), mTextSize(20)
    {
        mName = "TextView";
        setText(text);
    }
    
    void TextView::setText(const string text)
    {
        mText = text;
        invalidate();
    }
    
    HRect TextView::getSize()
    {
    	int bottom = 0;
    	int width = 0;
    	int height = 0;

#ifdef ANDROID
    	JNIEnv* mEnv = gJNIEnv;
    	gJVM->AttachCurrentThread(&mEnv, NULL);

		jclass mClsPaint = mEnv->FindClass("android/graphics/Paint");
    	jclass mClsRect = mEnv->FindClass("android/graphics/Rect");
    	jclass mClsString = mEnv->FindClass("java/lang/String");

    	jstring jText = mEnv->NewStringUTF(mText.c_str());

    	jmethodID stringLengthMid = mEnv->GetMethodID(mClsString, "length", "()I");
    	jint length = mEnv->CallIntMethod(jText, stringLengthMid);

    	jmethodID constructorRect = mEnv->GetMethodID(mClsRect, "<init>", "()V");
		jobject jrect = mEnv->NewObject(mClsRect, constructorRect);

		jmethodID constructorP = mEnv->GetMethodID(mClsPaint, "<init>", "()V");
		jobject mJPaint = mEnv->NewObject(mClsPaint, constructorP);

		jmethodID paintSetTextSizeMID = mEnv->GetMethodID(mClsPaint, "setTextSize", "(F)V");
		mEnv->CallVoidMethod(mJPaint, paintSetTextSizeMID, (jfloat)mTextSize);

		jmethodID mid = mEnv->GetMethodID(mClsPaint, "getTextBounds", "(Ljava/lang/String;IILandroid/graphics/Rect;)V");
		mEnv->CallVoidMethod(mJPaint, mid, jText, 0, length, jrect);

		jmethodID measureTextMID = mEnv->GetMethodID(mClsPaint, "measureText", "(Ljava/lang/String;)F");
		width = (int)(mEnv->CallFloatMethod(mJPaint, measureTextMID, jText) + 0.5);

		jmethodID midHeight = mEnv->GetMethodID(mClsRect, "height", "()I");
		height = mEnv->CallIntMethod(jrect, midHeight);
		jfieldID f = mEnv->GetFieldID(mClsRect, "bottom", "I");
		bottom = mEnv->GetIntField(jrect, f);

		mEnv->DeleteLocalRef(jText);
#elif __APPLE__
        UIFont *font = [UIFont boldSystemFontOfSize:mTextSize];
        NSDictionary *attributes = @{NSFontAttributeName: font};
        NSString* nsText = [NSString stringWithCString:mText.c_str() encoding:NSUTF8StringEncoding];
        CGSize fontSize = [nsText sizeWithAttributes:attributes];
        width = fontSize.width;
        height = fontSize.height;
#endif
        return HRect(0, bottom, width, height);
    }
    
    void TextView::setPosition(Vector2 pos, int halign, int valign)
    {
        mBound.mLeft = pos.x;
        mBound.mTop = pos.y;
        mBound.mWidth = mImage->getWidth();
        mBound.mHeight = mImage->getHeight();
        View::setBound(mBound, halign, valign);
    }

    void TextView::invalidate()
    {
    	HRect bound = getSize();
		mBound.mWidth = bound.mWidth;
		mBound.mHeight = bound.mHeight;
		ImagePtr img = mImage;
        if (!img || img->getWidth() != mBound.mWidth || img->getHeight() != mBound.mHeight)
        {
            img = ImageFactory::createImage(mBound.mWidth, mBound.mHeight);
            setImage(img);
            requestLayout();
        }
        mCanvas = CanvasFactory::getCanvas(img);
		mCanvas->setFontSize(mTextSize);
        mCanvas->drawColor(0);
        mCanvas->setColor(mTextColor);
		mCanvas->drawString(mText, 0, mBound.mHeight - bound.mTop);
        
        View::setBound(mBound, mHAlignment, mVAlignment);
		View::invalidate();
    }
}
