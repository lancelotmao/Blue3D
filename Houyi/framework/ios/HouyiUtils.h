//
//  HouyiUtils.h
//  Houyi
//
//  Created by Lance Mao on 23/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//
#ifndef HouyiUtils_H
#define HouyiUtils_H

#import <Foundation/Foundation.h>

#include "HouyiPass.h"
#include "HouyiRenderer.h"

using namespace Houyi;

@interface HouyiUtils : NSObject

// load data into memory
// TODO should also support input stream
+ (NSData*)getDataFromFile:(NSString*) fileName;

// use this function when have custom shader
+ (Pass*)loadShader:(Renderer*)renderer :(NSString*) vertexShader : (NSString*) fragmentShader;

@end

#endif
