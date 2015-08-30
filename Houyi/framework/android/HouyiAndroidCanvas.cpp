/*
 * HouyiAndroidCanvas.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: lancemao
 */

#include "HouyiAndroidCanvas.h"
#include "HouyiLog.h"
#include "HouyiMath.h"

namespace Houyi
{
    AndroidCanvas::AndroidCanvas(ImagePtr image) : Canvas(image), mEnv(0), mJPaint(0), mJCanvas(0),
            mClsCanvas(0), mClsPaint(0), mClsRect(0)
    {
        if (!gJVM || !gJNIEnv)
        {
            LOGE("AndroidCanvas::AndroidCanvas. invalid JNI");
            return;
        }

        mEnv = gJNIEnv;
        gJVM->AttachCurrentThread(&mEnv, NULL);

        mClsCanvas = mEnv->FindClass("android/graphics/Canvas");
        mClsRect = mEnv->FindClass("android/graphics/Rect");
        mClsPaint = mEnv->FindClass("android/graphics/Paint");

        jmethodID constructorP = mEnv->GetMethodID(mClsPaint, "<init>", "()V");
        mJPaint = mEnv->NewObject(mClsPaint, constructorP);
        if (!mJPaint)
        {
            LOGW("AndroidCanvas create paint failed");
        }

        jmethodID constructor = mEnv->GetMethodID(mClsCanvas, "<init>", "(Landroid/graphics/Bitmap;)V");
        mJCanvas = mEnv->NewObject(mClsCanvas, constructor, image->getBitmap());
        if (!mJCanvas)
        {
            LOGW("AndroidCanvas create canvas failed");
        }
    }

    void AndroidCanvas::drawColor(int color)
    {
        if (!gJVM || !mEnv)
        {
            LOGE("AndroidCanvas::drawColor. invalid JNI");
            return;
        }

        if (!mJCanvas)
        {
            LOGE("AndroidCanvas::drawColor. invalid Java Canvas");
            return;
        }

        gJVM->AttachCurrentThread(&mEnv, NULL);
        mClsCanvas = mEnv->FindClass("android/graphics/Canvas");

        if (((color >> 24) & 0xff) == 0)
        {
            jclass clsPorterDuff = mEnv->FindClass("android/graphics/PorterDuff$Mode");
            jfieldID fieldClear = mEnv->GetStaticFieldID(clsPorterDuff, "CLEAR", "Landroid/graphics/PorterDuff$Mode;");
            jobject jobjectClear = mEnv->GetStaticObjectField(clsPorterDuff, fieldClear);
            jmethodID mid = mEnv->GetMethodID(mClsCanvas, "drawColor", "(ILandroid/graphics/PorterDuff$Mode;)V");
            mEnv->CallVoidMethod(mJCanvas, mid, color, jobjectClear);
        }
        else
        {
            jmethodID mid = mEnv->GetMethodID(mClsCanvas, "drawColor", "(I)V");
            mEnv->CallVoidMethod(mJCanvas, mid, color);
        }
    }

    void AndroidCanvas::drawLine(float x1, float y1, float x2, float y2)
    {
        if (!gJVM || !mEnv)
        {
            LOGE("AndroidCanvas::drawLine. invalid JNI");
            return;
        }

        if (!mJCanvas)
        {
            LOGE("AndroidCanvas::drawLine. invalid Java Canvas");
            return;
        }

        gJVM->AttachCurrentThread(&mEnv, NULL);

        mClsCanvas = mEnv->FindClass("android/graphics/Canvas");
		mClsRect = mEnv->FindClass("android/graphics/Rect");
		mClsPaint = mEnv->FindClass("android/graphics/Paint");

		jmethodID paintSetColorMID = mEnv->GetMethodID(mClsPaint, "setColor", "(I)V");
		mEnv->CallVoidMethod(mJPaint, paintSetColorMID, mColor);

		jmethodID paintSetStrokeWidthMID = mEnv->GetMethodID(mClsPaint, "setStrokeWidth", "(F)V");
		mEnv->CallVoidMethod(mJPaint, paintSetStrokeWidthMID, mLineWidth);

		jmethodID mid = mEnv->GetMethodID(mClsCanvas, "drawLine", "(FFFFLandroid/graphics/Paint;)V");
		mEnv->CallVoidMethod(mJCanvas, mid, x1, y1, x2, y2, mJPaint);

		mEnv->DeleteLocalRef(mClsCanvas);
		mEnv->DeleteLocalRef(mClsRect);
		mEnv->DeleteLocalRef(mClsPaint);
    }

    void AndroidCanvas::drawRect(const HRect& rect)
    {
    	if (!gJVM || !mEnv)
		{
			LOGE("AndroidCanvas::fillRect. invalid JNI");
			return;
		}

		if (!mJCanvas)
		{
			LOGE("AndroidCanvas::fillRect. invalid Java Canvas");
			return;
		}

		gJVM->AttachCurrentThread(&mEnv, NULL);

		jmethodID paintSetColorMID = mEnv->GetMethodID(mClsPaint, "setColor", "(I)V");
		mEnv->CallVoidMethod(mJPaint, paintSetColorMID, mColor);

		jmethodID paintSetStyleMID = mEnv->GetMethodID(mClsPaint, "setStyle", "(Landroid/graphics/Paint$Style;)V");
		jclass clsStyle = mEnv->FindClass("android/graphics/Paint$Style");
		jfieldID fieldStyle = mEnv->GetStaticFieldID(clsStyle, "STROKE", "Landroid/graphics/Paint$Style;");
		jobject jobjectStyle = mEnv->GetStaticObjectField(clsStyle, fieldStyle);
		mEnv->CallVoidMethod(mJPaint, paintSetStyleMID, jobjectStyle);

		jmethodID constructorRect = mEnv->GetMethodID(mClsRect, "<init>", "(IIII)V");
		jobject jrect = mEnv->NewObject(mClsRect, constructorRect,
				(int)rect.mLeft, (int)rect.mTop, (int)(rect.mLeft+rect.mWidth), (int)(rect.mTop + rect.mHeight));
		if (!jrect)
		{
			LOGW("AndroidCanvas fillRect. create rect failed");
		}

		jmethodID mid = mEnv->GetMethodID(mClsCanvas, "drawRect", "(Landroid/graphics/Rect;Landroid/graphics/Paint;)V");
		mEnv->CallVoidMethod(mJCanvas, mid, jrect, mJPaint);
    }

    void AndroidCanvas::fillRect(const HRect& rect)
    {
        if (!gJVM || !mEnv)
        {
            LOGE("AndroidCanvas::fillRect. invalid JNI");
            return;
        }

        if (!mJCanvas)
        {
            LOGE("AndroidCanvas::fillRect. invalid Java Canvas");
            return;
        }

        gJVM->AttachCurrentThread(&mEnv, NULL);

        jmethodID paintSetColorMID = mEnv->GetMethodID(mClsPaint, "setColor", "(I)V");
        mEnv->CallVoidMethod(mJPaint, paintSetColorMID, mColor);

        jmethodID paintSetStyleMID = mEnv->GetMethodID(mClsPaint, "setStyle", "(Landroid/graphics/Paint$Style;)V");
		jclass clsStyle = mEnv->FindClass("android/graphics/Paint$Style");
		jfieldID fieldStyle = mEnv->GetStaticFieldID(clsStyle, "FILL", "Landroid/graphics/Paint$Style;");
		jobject jobjectStyle = mEnv->GetStaticObjectField(clsStyle, fieldStyle);
		mEnv->CallVoidMethod(mJPaint, paintSetStyleMID, jobjectStyle);

        jmethodID constructorRect = mEnv->GetMethodID(mClsRect, "<init>", "(IIII)V");
        jobject jrect = mEnv->NewObject(mClsRect, constructorRect,
                (int)rect.mLeft, (int)rect.mTop, (int)(rect.mLeft+rect.mWidth), (int)(rect.mTop + rect.mHeight));
        if (!jrect)
        {
            LOGW("AndroidCanvas fillRect. create rect failed");
        }

        jmethodID mid = mEnv->GetMethodID(mClsCanvas, "drawRect", "(Landroid/graphics/Rect;Landroid/graphics/Paint;)V");
        mEnv->CallVoidMethod(mJCanvas, mid, jrect, mJPaint);
    }

    void AndroidCanvas::drawString(const string& text, int x, int y, int hflag, int vflag)
    {
        if (!gJVM || !mEnv)
        {
            LOGE("AndroidCanvas::drawString. invalid JNI");
            return;
        }

        if (!mJCanvas)
        {
            LOGE("AndroidCanvas::drawString. invalid Java Canvas");
            return;
        }

        gJVM->AttachCurrentThread(&mEnv, NULL);
        jstring jText = mEnv->NewStringUTF(text.c_str());

        jmethodID mid = mEnv->GetMethodID(mClsCanvas, "drawText", "(Ljava/lang/String;FFLandroid/graphics/Paint;)V");

        jmethodID paintSetColorMID = mEnv->GetMethodID(mClsPaint, "setColor", "(I)V");
        mEnv->CallVoidMethod(mJPaint, paintSetColorMID, mColor);

        jmethodID paintSetTextSizeMID = mEnv->GetMethodID(mClsPaint, "setTextSize", "(F)V");
        mEnv->CallVoidMethod(mJPaint, paintSetTextSizeMID, (jfloat)mFontSize);

        jmethodID paintSetStyleMID = mEnv->GetMethodID(mClsPaint, "setStyle", "(Landroid/graphics/Paint$Style;)V");
		jclass clsStyle = mEnv->FindClass("android/graphics/Paint$Style");
		jfieldID fieldStyle = mEnv->GetStaticFieldID(clsStyle, "FILL", "Landroid/graphics/Paint$Style;");
		jobject jobjectStyle = mEnv->GetStaticObjectField(clsStyle, fieldStyle);
		mEnv->CallVoidMethod(mJPaint, paintSetStyleMID, jobjectStyle);

        jmethodID paintSetTextAlignMID = mEnv->GetMethodID(mClsPaint, "setTextAlign", "(Landroid/graphics/Paint$Align;)V");

        jclass clsAlign = mEnv->FindClass("android/graphics/Paint$Align");
        jfieldID fieldAlign = 0;
        if (hflag == EHAlignLeft)
        {
            fieldAlign = mEnv->GetStaticFieldID(clsAlign, "LEFT", "Landroid/graphics/Paint$Align;");
        }
        else if (hflag == EHAlignCenter)
        {
            fieldAlign = mEnv->GetStaticFieldID(clsAlign, "CENTER", "Landroid/graphics/Paint$Align;");
        }
        else
        {
            fieldAlign = mEnv->GetStaticFieldID(clsAlign, "RIGHT", "Landroid/graphics/Paint$Align;");
        }
        jobject jobjectAlign = mEnv->GetStaticObjectField(clsAlign, fieldAlign);
        mEnv->CallVoidMethod(mJPaint, paintSetTextAlignMID, jobjectAlign);

        jmethodID paintSetAntiAliasMID = mEnv->GetMethodID(mClsPaint, "setAntiAlias", "(Z)V");
        mEnv->CallVoidMethod(mJPaint, paintSetAntiAliasMID, true);

        jmethodID paintAscentMID = mEnv->GetMethodID(mClsPaint, "ascent", "()F");
        float textH = mEnv->CallFloatMethod(mJPaint, paintAscentMID);

        if (vflag == EVAlignTop)
        {
            y -= textH;
        }
        else if (vflag == EVAlignCenter)
        {
            y -= textH / 2;
        }
        mEnv->CallVoidMethod(mJCanvas, mid, jText, (jfloat)x, (jfloat)y, mJPaint);
        mEnv->DeleteLocalRef(jText);
    }

    void AndroidCanvas::drawImage(const ImagePtr image, const HRect& src, const HRect& dst)
    {
        if (!gJVM || !mEnv)
        {
            LOGE("AndroidCanvas::drawString. invalid JNI");
            return;
        }

        if (!mJCanvas)
        {
            LOGE("AndroidCanvas::drawString. invalid Java Canvas");
            return;
        }

        gJVM->AttachCurrentThread(&mEnv, NULL);

        mClsCanvas = mEnv->FindClass("android/graphics/Canvas");
        mClsRect = mEnv->FindClass("android/graphics/Rect");
        mClsPaint = mEnv->FindClass("android/graphics/Paint");

        jmethodID constructorP = mEnv->GetMethodID(mClsPaint, "<init>", "()V");
        mJPaint = mEnv->NewObject(mClsPaint, constructorP);

        jmethodID constructor = mEnv->GetMethodID(mClsCanvas, "<init>", "(Landroid/graphics/Bitmap;)V");
        mJCanvas = mEnv->NewObject(mClsCanvas, constructor, mImage->getBitmap());

        jmethodID cRotate = mEnv->GetMethodID(mClsCanvas, "rotate", "(F)V");
        jmethodID cTranslate = mEnv->GetMethodID(mClsCanvas, "translate", "(FF)V");
        jmethodID cScale = mEnv->GetMethodID(mClsCanvas, "scale", "(FF)V");
        mEnv->CallVoidMethod(mJCanvas, cTranslate, dst.mLeft+dst.mWidth/2, dst.mTop+dst.mHeight/2);
        mEnv->CallVoidMethod(mJCanvas, cRotate, mRotation * 180 / Math::PI);
        mEnv->CallVoidMethod(mJCanvas, cScale, mScaleX, mScaleY);
        mEnv->CallVoidMethod(mJCanvas, cTranslate, -dst.mWidth/2, -dst.mHeight/2);

        jmethodID constructorRect = mEnv->GetMethodID(mClsRect, "<init>", "(IIII)V");
        jobject srcRect = mEnv->NewObject(mClsRect, constructorRect,
                (int)src.mLeft, (int)src.mTop, (int)(src.mLeft+src.mWidth), (int)(src.mTop + src.mHeight));
        jobject dstRect = mEnv->NewObject(mClsRect, constructorRect,
                0, 0, (int)(dst.mWidth), (int)(dst.mHeight));
        if (!srcRect || !dstRect)
        {
            LOGW("AndroidCanvas drawImage. create rect failed");
        }

        jmethodID mid = mEnv->GetMethodID(mClsCanvas, "drawBitmap", "(Landroid/graphics/Bitmap;Landroid/graphics/Rect;Landroid/graphics/Rect;Landroid/graphics/Paint;)V");
        mEnv->CallVoidMethod(mJCanvas, mid, image->getBitmap(), srcRect, dstRect, mJPaint);
    }

    void AndroidCanvas::setPaintFontSize()
    {
        jmethodID paintSetTextSizeMID = mEnv->GetMethodID(mClsPaint, "setTextSize", "(F)V");
        mEnv->CallVoidMethod(mJPaint, paintSetTextSizeMID, (jfloat)mFontSize);
    }

    float AndroidCanvas::getStringWidth(const string& str)
    {
        if (!gJVM || !mEnv)
        {
            LOGE("AndroidCanvas::drawString. invalid JNI");
            return 0;
        }

        if (!mJCanvas)
        {
            LOGE("AndroidCanvas::drawString. invalid Java Canvas");
            return 0;
        }

        LOGE("%s", str.c_str());

        gJVM->AttachCurrentThread(&mEnv, NULL);
        jmethodID mid = mEnv->GetMethodID(mClsPaint, "measureText", "(Ljava/lang/String;)F");
        jstring jText = mEnv->NewStringUTF(str.c_str());
        float res = mEnv->CallFloatMethod(mJPaint, mid, jText);
        mEnv->DeleteLocalRef(jText);
        return res;
    }
}
