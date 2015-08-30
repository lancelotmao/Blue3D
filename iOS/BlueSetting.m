//
//  BlueSetting.m
//  Houyi
//
//  Created by Lance Mao on 22/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "BlueSetting.h"

@implementation BlueSetting

// note shading mode stores in C++
@synthesize mShowFPS, mShowGrd, mShowAxis, mShowAABB, mShowFourView, mRotateX, mRotateY;

static BlueSetting* mIns;

+(BlueSetting*)getInstance
{
    if (!mIns)
    {
        mIns = [BlueSetting new];
        mIns.mRotateX = YES;
        mIns.mRotateY = YES;
    }
    return mIns;
}

@end
