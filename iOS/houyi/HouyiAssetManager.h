//
//  HouyiAssetManager.h
//  Houyi
//
//  Created by Lance Mao on 17/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "HouyiLoadOperation.h"

using namespace Houyi;

@interface HouyiAssetManager : NSObject
{
    NSOperationQueue* queue;
}

@property (atomic, strong) id<LoadDelegate> loadDelegate;

+ (HouyiAssetManager*) getInstance :(id<LoadDelegate>)delegate;
- (id)init;
- (void)cancelAll;
- (void)cancelInActive :(NSRange)activeRange;
- (BOOL)isLoading;
- (LoadOperation*)getOperation :(NSString*)path;

- (void)loadAsset:(NSString*)assetName;
- (void)loadAsset:(NSString*)assetName focusIndex:(int)index;
- (Scene*)loadAssetSync:(NSString*)assetName;

@end
