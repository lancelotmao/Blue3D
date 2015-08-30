#ifndef HRECT_H
#define HRECT_H

class HRect
{
public:
    HRect() : mLeft(0), mTop(0), mWidth(0), mHeight(0)
    {
    }
    
    HRect(float left, float top, float width, float height)
        : mLeft(left), mTop(top), mWidth(width), mHeight(height)
    {

    }

    bool operator==(const HRect& rect)
    {
        return mLeft == rect.mLeft && mTop == rect.mTop
                && mWidth == rect.mWidth && mHeight == rect.mHeight;
    }

    bool operator!=(const HRect& rect)
    {
        return !(*this == rect);
    }
    
    void expand(float value)
    {
        mLeft -= value;
        mTop -= value;
        mWidth += 2 * value;
        mHeight += 2 * value;
    }

    float mLeft;
    float mTop;
    float mWidth;
    float mHeight;
};

#endif
