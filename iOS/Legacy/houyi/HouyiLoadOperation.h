//
//  HouyiLoadOperation.h
//  Houyi
//
//  Created by Lance Mao on 21/5/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <Houyi/Houyi.h>

using namespace Houyi;

@class LoadOperation;

@protocol LoadDelegate <NSObject>
@required
- (void)onLoadFinished:(LoadOperation*)op :(Scene*)scene;
@end

@interface LoadOperation : NSOperation
{
    Loader* loader;
}

@property (readwrite, nonatomic) int index;
@property (strong, nonatomic) NSString* fileName;
@property (strong, nonatomic) id<LoadDelegate> delegate;

// override and we will cancel loading
- (void)cancel;
- (float)getProgress;

@end
