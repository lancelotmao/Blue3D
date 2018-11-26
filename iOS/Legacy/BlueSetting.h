//
//  BlueSetting.h
//  Houyi
//
//  Created by Lance Mao on 22/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BlueSetting : NSObject
{
}

@property (nonatomic, readwrite) bool mShowFPS;
@property (nonatomic, readwrite) bool mShowGrd;
@property (nonatomic, readwrite) bool mShowAxis;
@property (nonatomic, readwrite) bool mShowAABB;
@property (nonatomic, readwrite) bool mShowFourView;
@property (nonatomic, readwrite) bool mRotateX;
@property (nonatomic, readwrite) bool mRotateY;

+(BlueSetting*)getInstance;

@end
