//
//  HouyiiOSCanvas.cpp
//  Houyi
//
//  Created by Lance Mao on 19/11/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "Houyi.h"

namespace Houyi
{
    iOSCanvas::iOSCanvas(ImagePtr image) : Canvas(image)
    {
        int width = mImage->getWidth();
        int height = mImage->getHeight();
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        size_t bitsPerComponent = 8;
        size_t bytesPerPixel    = 4;
        size_t bytesPerRow      = (width * bitsPerComponent * bytesPerPixel + 7) / 8;
        size_t dataSize         = bytesPerRow * height;
        
        mData = (unsigned char*)malloc(dataSize);
        memset(mData, 0, dataSize);
        
        mContext = CGBitmapContextCreate(mData, width, height,
                                             bitsPerComponent,
                                             bytesPerRow, colorSpace,
                                             kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
        
        CGContextDrawImage(mContext, CGRectMake(0, 0, width, height), image->getUIImage().CGImage);
        
        CGColorSpaceRelease(colorSpace);
    }
    
    iOSCanvas::~iOSCanvas()
    {
        CGContextRelease(mContext);
        free(mData);
    }
    
    void iOSCanvas::drawColor(int color)
    {
        mColor = color;
        fillRect(HRect(0, 0, mImage->getWidth(), mImage->getHeight()));
    }
    
    void iOSCanvas::drawLine(float x1, float y1, float x2, float y2)
    {
        float a = ((mColor >> 24) &0xff) / 255.0;
        float r = ((mColor >> 16) & 0xff) / 255.0;
        float g = ((mColor >> 8) & 0xff) / 255.0;
        float b = (mColor & 0xff) / 255.0;
        CGContextSetRGBStrokeColor(mContext, r, g, b, a);
        CGContextSetLineWidth(mContext, mLineWidth);
        CGContextMoveToPoint(mContext, x1, mImage->getHeight()-y1);
        CGContextAddLineToPoint(mContext, x2, mImage->getHeight()-y2);
        CGContextStrokePath(mContext);
        
        CGImageRef imageRef = CGBitmapContextCreateImage(mContext);
        UIImage *result = [UIImage imageWithCGImage:imageRef];
        CGImageRelease(imageRef);
        
        mImage->setUIImage(result);
    }

    void iOSCanvas::drawRect(const HRect& rect)
    {
        int height = mImage->getHeight();
        CGRect myRect = {rect.mLeft, rect.mTop, rect.mWidth, rect.mHeight};
        float a = ((mColor >> 24) &0xff) / 255.0;
        float r = ((mColor >> 16) & 0xff) / 255.0;
        float g = ((mColor >> 8) & 0xff) / 255.0;
        float b = (mColor & 0xff) / 255.0;
        CGContextSetRGBFillColor(mContext, r, g, b, a);
        CGContextSetRGBStrokeColor(mContext, r, g, b, a);
        CGContextSaveGState(mContext);
        CGContextTranslateCTM(mContext, 0.0f, height);
        CGContextScaleCTM(mContext, 1.0f, -1.0f);
        CGContextStrokeRect(mContext, myRect);
        CGContextRestoreGState(mContext);
        
        CGImageRef imageRef = CGBitmapContextCreateImage(mContext);
        UIImage *result = [UIImage imageWithCGImage:imageRef];
        CGImageRelease(imageRef);
        mImage->setUIImage(result);
    }
    
    void iOSCanvas::fillRect(const HRect& rect)
    {
        int height = mImage->getHeight();
        CGRect myRect = {rect.mLeft, rect.mTop, rect.mWidth, rect.mHeight};
        float a = ((mColor >> 24) &0xff) / 255.0;
        float r = ((mColor >> 16) & 0xff) / 255.0;
        float g = ((mColor >> 8) & 0xff) / 255.0;
        float b = (mColor & 0xff) / 255.0;
        if (a == 0)
        {
            CGContextClearRect(mContext, myRect);
        }
        CGContextSetRGBFillColor(mContext, r, g, b, a);
        CGContextSetRGBStrokeColor(mContext, r, g, b, a);
        CGContextSaveGState(mContext);
        CGContextTranslateCTM(mContext, 0.0f, height);
        CGContextScaleCTM(mContext, 1.0f, -1.0f);
        CGContextFillRect(mContext, myRect);
        CGContextRestoreGState(mContext);

        CGImageRef imageRef = CGBitmapContextCreateImage(mContext);
        UIImage *result = [UIImage imageWithCGImage:imageRef];
        CGImageRelease(imageRef);
        mImage->setUIImage(result);
    }

    void iOSCanvas::drawString(const string& text, int x, int y, int hflag, int vflag)
    {
        UIFont *font = [UIFont boldSystemFontOfSize:mFontSize];
        int height = mImage->getHeight();
        
        NSDictionary *attributes = @{NSFontAttributeName: font};
        NSString* nsText = [NSString stringWithCString:text.c_str() encoding:NSUTF8StringEncoding];
        CGSize fontSize = [nsText sizeWithAttributes:attributes];
        
        if (hflag == EHAlignCenter)
        {
            x -= fontSize.width / 2;
        }
        else if (hflag == EHAlignRight)
        {
            x -= fontSize.width;
        }
        
        if (vflag == EVAlignCenter)
        {
            y -= fontSize.height / 2;
        }
        else if (vflag == EVAlignBottom)
        {
            y -= fontSize.height;
        }
        
        y = height - y;
        
        float a = ((mColor >> 24) &0xff) / 255.0;
        float r = ((mColor >> 16) & 0xff) / 255.0;
        float g = ((mColor >> 8) & 0xff) / 255.0;
        float b = (mColor & 0xff) / 255.0;
        UIColor* color = [UIColor colorWithRed:r green:g blue:b alpha:a];

        NSMutableParagraphStyle *textStyle = [[NSMutableParagraphStyle defaultParagraphStyle] mutableCopy];
        NSDictionary *dictionary = @{ NSFontAttributeName: font, NSParagraphStyleAttributeName: textStyle, NSForegroundColorAttributeName: color};
        
        UIGraphicsPushContext(mContext);
        CGContextSaveGState(mContext);
        CGContextTranslateCTM(mContext, 0.0f, height);
        CGContextScaleCTM(mContext, 1.0f, -1.0f);
        [nsText drawAtPoint:CGPointMake(x, height - y) withAttributes:dictionary];
        CGContextRestoreGState(mContext);
        UIGraphicsPopContext();
        
        CGImageRef imageRef = CGBitmapContextCreateImage(mContext);
        UIImage *result = [UIImage imageWithCGImage:imageRef];
        CGImageRelease(imageRef);
        
        mImage->setUIImage(result);
    }
    
    void iOSCanvas::drawImage(const ImagePtr image, const HRect& src, const HRect& dst)
    {
        if (!image)
        {
            return;
        }
        
        CGRect srcRect = CGRectMake(src.mLeft, src.mTop, src.mWidth, src.mHeight);
        CGImageRef croppedRef = CGImageCreateWithImageInRect([image->getUIImage() CGImage], srcRect);
        UIImage* cropped = [UIImage imageWithCGImage:croppedRef];
        CGImageRelease(croppedRef);
        
        CGContextSaveGState(mContext);
        float bufferHeight = mImage->getHeight();
        CGContextTranslateCTM(mContext, dst.mLeft + dst.mWidth/2,
                              bufferHeight - dst.mTop - dst.mHeight + dst.mHeight/2);
        // iOS rotation winding is opposite as of android
        CGContextRotateCTM(mContext, -mRotation);
        CGContextScaleCTM(mContext, mScaleX, mScaleY);
        CGContextTranslateCTM(mContext, -dst.mWidth/2, -dst.mHeight/2);
        
        CGContextDrawImage(mContext, CGRectMake(0, 0, dst.mWidth, dst.mHeight), cropped.CGImage);
        CGContextRestoreGState(mContext);
        
        CGImageRef imageRef = CGBitmapContextCreateImage(mContext);
        UIImage *result = [UIImage imageWithCGImage:imageRef];
        CGImageRelease(imageRef);
    
        mImage->setUIImage(result);
    }
    
    float iOSCanvas::getStringWidth(const string& str)
    {
        NSString* nsText = [NSString stringWithCString:str.c_str() encoding:NSUTF8StringEncoding];
        UIFont* font = [UIFont boldSystemFontOfSize:mFontSize];
        NSDictionary *attributes = @{NSFontAttributeName: font};
        CGSize size = [nsText sizeWithAttributes:attributes];
        return size.width;
    }
}
