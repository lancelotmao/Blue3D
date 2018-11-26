//
//  DataManager.h
//  Houyi
//
//  Created by Lance Mao on 14/5/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "HouyiLoadOperation.h"
#include <Houyi/Houyi.h>

@protocol FocusDelegate <NSObject>

@required

-(void)onFocusChanged :(int)newFocus :(int)oldFocus;
-(void)onFocusModelLoaded :(int)newFocus :(Scene*)scene;

@end

@interface DataManager : NSObject<LoadDelegate>
{
}

@property (atomic, readwrite) int itemCount;
@property (atomic, readwrite) int focus;
@property (atomic, readwrite) int cacheSize;
@property (atomic, strong) NSMutableArray* items;
@property (atomic, strong) id<FocusDelegate> focusDelegate;

+ (DataManager*)getInstance;
- (void)addPrefetchTask;
- (id)getFocusItem;
- (float)getFocusProgress;

- (void)changeFocus:(int)newFocus; // delibrately avoid setter
- (void)onLoadFinished:(LoadOperation*)op :(Scene*)scene;

@end
