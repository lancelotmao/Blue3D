//
//  UIImage+HouyiUIImage.m
//  Houyi
//
//  Created by Lance Mao on 15/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "UIImage+HouyiUIImage.h"

CGFloat DegreesToRadians(CGFloat degrees) {return degrees * M_PI / 180;};
CGFloat RadiansToDegrees(CGFloat radians) {return radians * 180/M_PI;};

@implementation UIImage (HouyiUIImage)

-(UIImage *)imageAtRect:(CGRect)rect
{
    
    CGImageRef imageRef = CGImageCreateWithImageInRect([self CGImage], rect);
    UIImage* subImage = [UIImage imageWithCGImage: imageRef];
    CGImageRelease(imageRef);
    
    return subImage;
    
}

- (UIImage *)imageByScalingProportionallyToMinimumSize:(CGSize)targetSize {
    
    UIImage *sourceImage = self;
    UIImage *newImage = nil;
    
    CGSize imageSize = sourceImage.size;
    CGFloat width = imageSize.width;
    CGFloat height = imageSize.height;
    
    CGFloat targetWidth = targetSize.width;
    CGFloat targetHeight = targetSize.height;
    
    CGFloat scaleFactor = 0.0;
    CGFloat scaledWidth = targetWidth;
    CGFloat scaledHeight = targetHeight;
    
    CGPoint thumbnailPoint = CGPointMake(0.0,0.0);
    
    if (CGSizeEqualToSize(imageSize, targetSize) == NO) {
        
        CGFloat widthFactor = targetWidth / width;
        CGFloat heightFactor = targetHeight / height;
        
        if (widthFactor > heightFactor)
            scaleFactor = widthFactor;
        else
            scaleFactor = heightFactor;
        
        scaledWidth  = width * scaleFactor;
        scaledHeight = height * scaleFactor;
        
        // center the image
        
        if (widthFactor > heightFactor) {
            thumbnailPoint.y = (targetHeight - scaledHeight) * 0.5;
        } else if (widthFactor < heightFactor) {
            thumbnailPoint.x = (targetWidth - scaledWidth) * 0.5;
        }
    }
    
    
    // this is actually the interesting part:
    
    UIGraphicsBeginImageContext(targetSize);
    
    CGRect thumbnailRect = CGRectZero;
    thumbnailRect.origin = thumbnailPoint;
    thumbnailRect.size.width  = scaledWidth;
    thumbnailRect.size.height = scaledHeight;
    
    [sourceImage drawInRect:thumbnailRect];
    
    newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    if(newImage == nil) NSLog(@"could not scale image");
    
    
    return newImage ;
}


- (UIImage *)imageByScalingProportionallyToSize:(CGSize)targetSize {
    
    UIImage *sourceImage = self;
    UIImage *newImage = nil;
    
    CGSize imageSize = sourceImage.size;
    CGFloat width = imageSize.width;
    CGFloat height = imageSize.height;
    
    CGFloat targetWidth = targetSize.width;
    CGFloat targetHeight = targetSize.height;
    
    CGFloat scaleFactor = 0.0;
    CGFloat scaledWidth = targetWidth;
    CGFloat scaledHeight = targetHeight;
    
    CGPoint thumbnailPoint = CGPointMake(0.0,0.0);
    
    if (CGSizeEqualToSize(imageSize, targetSize) == NO) {
        
        CGFloat widthFactor = targetWidth / width;
        CGFloat heightFactor = targetHeight / height;
        
        if (widthFactor < heightFactor)
            scaleFactor = widthFactor;
        else
            scaleFactor = heightFactor;
        
        scaledWidth  = width * scaleFactor;
        scaledHeight = height * scaleFactor;
        
        // center the image
        
        if (widthFactor < heightFactor) {
            thumbnailPoint.y = (targetHeight - scaledHeight) * 0.5;
        } else if (widthFactor > heightFactor) {
            thumbnailPoint.x = (targetWidth - scaledWidth) * 0.5;
        }
    }
    
    
    // this is actually the interesting part:
    
    UIGraphicsBeginImageContext(targetSize);
    
    CGRect thumbnailRect = CGRectZero;
    thumbnailRect.origin = thumbnailPoint;
    thumbnailRect.size.width  = scaledWidth;
    thumbnailRect.size.height = scaledHeight;
    
    [sourceImage drawInRect:thumbnailRect];
    
    newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    if(newImage == nil) NSLog(@"could not scale image");
    
    
    return newImage ;
}


- (UIImage *)imageByScalingToSize:(CGSize)targetSize {
    
    UIImage *sourceImage = self;
    UIImage *newImage = nil;
    
    //   CGSize imageSize = sourceImage.size;
    //   CGFloat width = imageSize.width;
    //   CGFloat height = imageSize.height;
    
    CGFloat targetWidth = targetSize.width;
    CGFloat targetHeight = targetSize.height;
    
    //   CGFloat scaleFactor = 0.0;
    CGFloat scaledWidth = targetWidth;
    CGFloat scaledHeight = targetHeight;
    
    CGPoint thumbnailPoint = CGPointMake(0.0,0.0);
    
    // this is actually the interesting part:
    
    UIGraphicsBeginImageContext(targetSize);
    
    CGRect thumbnailRect = CGRectZero;
    thumbnailRect.origin = thumbnailPoint;
    thumbnailRect.size.width  = scaledWidth;
    thumbnailRect.size.height = scaledHeight;
    
    [sourceImage drawInRect:thumbnailRect];
    
    newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    if(newImage == nil) NSLog(@"could not scale image");
    
    
    return newImage ;
}


- (UIImage *)imageRotatedByRadians:(CGFloat)radians
{
    return [self imageRotatedByDegrees:RadiansToDegrees(radians)];
}

- (UIImage *)imageRotatedByDegrees:(CGFloat)degrees
{
    CGImageRef img = self.CGImage;
    size_t width = CGImageGetWidth(img);
    size_t height = CGImageGetHeight(img);
    
    // calculate the size of the rotated view's containing box for our drawing space
    UIView *rotatedViewBox = [[UIView alloc] initWithFrame:CGRectMake(0,0,width, height)];
    CGAffineTransform t = CGAffineTransformMakeRotation(DegreesToRadians(degrees));
    rotatedViewBox.transform = t;
    CGSize rotatedSize = rotatedViewBox.frame.size;
    
    // Create the bitmap context
    UIGraphicsBeginImageContext(rotatedSize);
    CGContextRef bitmap = UIGraphicsGetCurrentContext();
    
    // Move the origin to the middle of the image so we will rotate and scale around the center.
    CGContextTranslateCTM(bitmap, rotatedSize.width/2, rotatedSize.height/2);
    
    //   // Rotate the image context
    CGContextRotateCTM(bitmap, DegreesToRadians(degrees));
    
    // Now, draw the rotated/scaled image into the context
    CGContextScaleCTM(bitmap, 1.0, -1.0);
    CGContextDrawImage(bitmap, CGRectMake(-width / 2, -height / 2, width, height), [self CGImage]);
    
    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return newImage;
    
}

// handles images from camera when above rotate method fails
// 0: target is portrait up; rotate to left or PI/2
// 1: taregt is portrait up side down; rotate to right or -PI/2
// 2: target is landscape right
- (UIImage*)rotateData:(UIImage *)image :(int)type
{
    if (type == 2)
    {
        return [self rotate180:image];
    }
    
    CGImageRef img = image.CGImage;
    size_t width = CGImageGetWidth(img);
    size_t height = CGImageGetHeight(img);
    
    CFDataRef srcref = CGDataProviderCopyData(CGImageGetDataProvider(img));
    UInt8 *src = (UInt8 *)CFDataGetBytePtr(srcref);
    CFDataRef dataref = CGDataProviderCopyData(CGImageGetDataProvider(img));
    UInt8 *data = (UInt8 *)CFDataGetBytePtr(dataref);
    int length = CFDataGetLength(dataref);
    
    for (int i = 0;i < length;i+=4)
    {
        int pixelIndex = i / 4;
        int row = pixelIndex / width;
        int col = pixelIndex % width;
        int targetIndex = type == 0 ? (col * height + height - 1 - row) * 4 : ((width - 1 - col) * height + row) * 4;
        memcpy(data + targetIndex, src + i, 4 * sizeof(UInt8));
    }
    
    size_t bitsPerComponent = CGImageGetBitsPerComponent(img);
    size_t bitsPerPixel = CGImageGetBitsPerPixel(img);
    size_t bytesPerRow = height * (bitsPerPixel / bitsPerComponent);
    CGColorSpaceRef colorspace = CGImageGetColorSpace(img);
    CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(img);
    CFDataRef newData = CFDataCreate(NULL,(UInt8*)data,length);
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(newData);
    CGImageRef newImg = CGImageCreate(height, width, bitsPerComponent,bitsPerPixel,bytesPerRow,colorspace,bitmapInfo,provider,NULL,true,kCGRenderingIntentDefault);
    
    UIImage* res = [UIImage imageWithCGImage:newImg];
    // release
    CGImageRelease(newImg);
    CFRelease(srcref);
    CFRelease(dataref);
    CFRelease(newData);
    CGDataProviderRelease(provider);
    
    return res;
}

- (UIImage*)rotate180:(UIImage *)image
{
    CGImageRef img = image.CGImage;
    size_t width = CGImageGetWidth(img);
    size_t height = CGImageGetHeight(img);

    CFDataRef dataref = CGDataProviderCopyData(CGImageGetDataProvider(img));
    UInt8 *data = (UInt8 *)CFDataGetBytePtr(dataref);
    int length = CFDataGetLength(dataref);
    
    
    for (int i = 0;i < length / 2;i+=4)
    {
        int pixelIndex = i / 4;
        int row = pixelIndex / width;
        int col = pixelIndex % width;
        int targetIndex = ((height - 1 - row) * width + width - 1 - col) * 4;
        
        UInt8 tmp[4];
        memcpy(tmp, data + i, 4 * sizeof(UInt8));
        memcpy(data + i, data + targetIndex, 4 * sizeof(UInt8));
        memcpy(data + targetIndex, tmp, 4 * sizeof(UInt8));
    }
    
    size_t bitsPerComponent = CGImageGetBitsPerComponent(img);
    size_t bitsPerPixel = CGImageGetBitsPerPixel(img);
    size_t bytesPerRow = width * (bitsPerPixel / bitsPerComponent);
    CGColorSpaceRef colorspace = CGImageGetColorSpace(img);
    CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(img);
    CFDataRef newData = CFDataCreate(NULL,(UInt8*)data,length);
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(newData);
    CGImageRef newImg = CGImageCreate(width, height, bitsPerComponent,bitsPerPixel,bytesPerRow,colorspace,bitmapInfo,provider,NULL,true,kCGRenderingIntentDefault);
    
    UIImage* res = [UIImage imageWithCGImage:newImg];
    // release
    CGImageRelease(newImg);
    CFRelease(dataref);
    CFRelease(newData);
    CGDataProviderRelease(provider);
    
    return res;
}

- (UIImage*)removeOrientationOfImage
{
    CGImageRef img=self.CGImage;
    size_t width=CGImageGetWidth(img);
    size_t height=CGImageGetHeight(img);

    CFDataRef dataref = CGDataProviderCopyData(CGImageGetDataProvider(img));
    UInt8 *data = (UInt8 *)CFDataGetBytePtr(dataref);
    int length = CFDataGetLength(dataref);
    
    size_t bitsPerComponent = CGImageGetBitsPerComponent(img);
    size_t bitsPerPixel = CGImageGetBitsPerPixel(img);
    size_t bytesPerRow = width * (bitsPerPixel / bitsPerComponent);
    CGColorSpaceRef colorspace=CGImageGetColorSpace(img);
    CGBitmapInfo bitmapInfo=CGImageGetBitmapInfo(img);
    CFDataRef newData=CFDataCreate(NULL,(UInt8*)data,length);
    CGDataProviderRef provider=CGDataProviderCreateWithCFData(newData);
    CGImageRef newImg=CGImageCreate(width, height, bitsPerComponent,bitsPerPixel,bytesPerRow,colorspace,bitmapInfo,provider,NULL,true,kCGRenderingIntentDefault);
    
    UIImage* res = [UIImage imageWithCGImage:newImg];
    // release
    CGImageRelease(newImg);
    CFRelease(dataref);
    CFRelease(newData);
    CGDataProviderRelease(provider);
    return res;
}

@end;
