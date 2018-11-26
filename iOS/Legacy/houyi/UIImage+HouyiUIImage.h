//
//  UIImage+HouyiUIImage.h
//  Houyi
//
//  Created by Lance Mao on 15/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (HouyiUIImage)

- (UIImage *)imageAtRect:(CGRect)rect;
- (UIImage *)imageByScalingProportionallyToMinimumSize:(CGSize)targetSize;
- (UIImage *)imageByScalingProportionallyToSize:(CGSize)targetSize;
- (UIImage *)imageByScalingToSize:(CGSize)targetSize;
- (UIImage *)imageRotatedByRadians:(CGFloat)radians;
- (UIImage *)imageRotatedByDegrees:(CGFloat)degrees;
- (UIImage*)rotateData:(UIImage *)image :(int)type;
- (UIImage*)removeOrientationOfImage;

@end
