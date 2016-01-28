//
//  Houyi3D.h
//  Houyi
//
//  Created by lancemao on 9/15/15.
//  Copyright (c) 2015 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Houyi.h"

@interface HouyiRoot : NSObject
- (void)sRoot:(Root*)root;
- (Root*)gRoot;
@end

@interface Houyi3D : NSObject

- (HouyiRoot*)startEngine;
- (void)render:(HouyiRoot*)root;
- (BOOL)isRenderRequested;
- (void)requestRender:(BOOL)requested;

- (void)onWindowChanged:(HouyiRoot*)root :(NSNumber*)width :(NSNumber*)height;
- (void)loadDataSync:(NSString*)filePath;

@end
