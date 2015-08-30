#include "HouyiCanvas.h"

namespace Houyi
{
    Canvas::Canvas(ImagePtr image) : mImage(image), mColor(0xffffffff), mFontSize(20), mLineWidth(1),
            mRotation(0), mScaleX(1), mScaleY(1)
    {

    }

    Canvas::~Canvas()
    {

    }

    void Canvas::setColor(int color)
    {
        mColor = color;
    }

    void Canvas::setFontSize(int size)
    {
        mFontSize = size;
    }

    void Canvas::setLineWidth(float size)
    {
        mLineWidth = size;
    }

    void Canvas::drawStringWrapped(const string& text, int borderX, int borderY, WrapMode wrapMode)
    {
        setPaintFontSize();

        static const int max_split_count = 128;
        string split[max_split_count];

        int split_count = 0;
        int start = 0;
        string tmp = "";
        int str_length = text.length();
        for (int i = 0;i < str_length;++i)
        {
            char c = text[i];
            float w = getStringWidth(tmp + c);
            if (w >= mImage->getWidth())
            {
                split[split_count] = tmp;
                tmp = "";
                split_count++;
                start = i;
                if (w > mImage->getWidth())
                {
                    i--;
                }
            }
            else
            {
                tmp += c;
            }
        }
        // get last row. i.e. from start to length - 1
        split[split_count++] = text.substr(start, str_length);

        for (int i = 0;i < split_count;++i)
        {
            drawString(split[i], borderX, borderY + mFontSize * (i + 1));
        }
    }

    void Canvas::drawImageFill(const ImagePtr image)
    {
        float w = mImage->getWidth();
        float h = mImage->getHeight();
        float iw = image->getWidth();
        float ih = image->getHeight();
        drawImage(image, HRect(0, 0, iw, ih), HRect(0, 0, w, h));
    }
    
    void Canvas::drawImageCropCenter(const ImagePtr image)
    {
        float srcX = 0;
        float srcY = 0;
        float width = mImage->getWidth();
        float height = mImage->getHeight();
        float w = image->getWidth();
        float h = image->getHeight();
        if (width / w > height / h)
        {
            srcY = (h - height / width * w) / 2;
        }
        else
        {
            srcX = (w - width / height * h) / 2;
        }

        drawImage(image, HRect(srcX, srcY, w-2*srcX, h-2*srcY), HRect(0, 0, width, height));
    }
    
    void Canvas::drawImageFitCenter(const ImagePtr image)
    {
        float offsetX = 0;
        float offsetY = 0;
        float w = mImage->getWidth();
        float h = mImage->getHeight();
        float iw = image->getWidth();
        float ih = image->getHeight();
        int dstW = w;
        int dstH = h;
        if (w / h > iw / ih)
        {
            dstW = iw / ih * h;
            offsetX = (w - dstW) / 2;
        }
        else
        {
            dstH = ih / iw * w;
            offsetY = (h - dstH) / 2;
        }
        drawImage(image, HRect(0, 0, iw, ih), HRect(offsetX, offsetY, dstW, dstH));
    }

    void Canvas::setRotate(float radian)
    {
        mRotation = radian;
    }

    void Canvas::setScaleX(float sx)
    {
        mScaleX = sx;
    }

    void Canvas::setScaleY(float sy)
    {
        mScaleY = sy;
    }
}
