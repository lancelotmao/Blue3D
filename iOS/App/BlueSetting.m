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
//@synthesize mShowFPS, mShowGrd, mShowAxis, mShowAABB, mShowFourView, mRotateX, mRotateY;

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

- (void)setMShowFPS:(bool)showFPS {
    [[NSUserDefaults standardUserDefaults] setBool:showFPS forKey:@"ShowFPS"];
}

- (bool)mShowFPS {
    return [[NSUserDefaults standardUserDefaults] boolForKey:@"ShowFPS"];
}

- (void)setMShowGrid:(bool)showGrid {
    [[NSUserDefaults standardUserDefaults] setBool:showGrid forKey:@"showGrid"];
}

- (bool)mShowGrid {
    return [[NSUserDefaults standardUserDefaults] boolForKey:@"showGrid"];
}

- (void)setMShowAxis:(bool)mShowAxis {
    [[NSUserDefaults standardUserDefaults] setBool:mShowAxis forKey:@"mShowAxis"];
}

- (bool)mShowAxis {
    return [[NSUserDefaults standardUserDefaults] boolForKey:@"mShowAxis"];
}

- (void)setMShowAABB:(bool)mShowAABB {
    [[NSUserDefaults standardUserDefaults] setBool:mShowAABB forKey:@"mShowAABB"];
}

- (bool)mShowAABB {
    return [[NSUserDefaults standardUserDefaults] boolForKey:@"mShowAABB"];
}

@end
