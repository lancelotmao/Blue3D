//
//  HouyiAssetManager.m
//  Houyi
//
//  Created by Lance Mao on 17/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiAssetManager.h"
#import "HouyiViewController.h"

@implementation HouyiAssetManager

@synthesize loadDelegate;

static HouyiAssetManager* ins = nil;

+ (HouyiAssetManager*) getInstance :(id<LoadDelegate>)delegate
{
    if (ins == nil) {
        ins = [HouyiAssetManager new];
    }
    ins.loadDelegate = delegate;
    return ins;
}

- (id)init
{
    self = [super init];
    if(self)
    {
        queue = [NSOperationQueue new];
    }
    return self;
}

- (void)cancelAll
{
    [queue cancelAllOperations];
}

- (void)cancelInActive :(NSRange)activeRange
{
    int start = activeRange.location;
    int length = activeRange.length;
    for (int i = 0;i < [queue.operations count];++i) {
        LoadOperation* op = queue.operations[i];
        if (op.index < start || op.index >= start + length) {
            [op cancel];
        }
    }
}

- (BOOL)isLoading
{
    return [queue operationCount] > 0;
}

- (LoadOperation*)getOperation :(NSString*)path
{
    for (int i = 0;i < [queue.operations count];++i) {
        LoadOperation* op = queue.operations[i];
        if ([op.fileName isEqualToString:path]) {
            return op;
        }
    }
    return 0;
}

- (void)loadAsset:(NSString*)assetName
{
    [self loadAsset:assetName focusIndex:0];
}

- (void)loadAsset:(NSString*)assetName focusIndex:(int)index;
{
    NSString* cache = [assetName stringByAppendingString:@".houyi"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:cache]) {
        assetName = cache;
    }

    @synchronized(queue.operations) {
        for (int i = 0;i < [queue.operations count];++i) {
            LoadOperation* op = [queue.operations objectAtIndex:i];
            if (op && [op.fileName isEqualToString:assetName]) {
                // already in operation queue. skip
                return;
            }
        }
        LoadOperation *op = [LoadOperation new];
        op.fileName = assetName;
        op.index = index;
        op.delegate = self.loadDelegate;
        [queue addOperation:op];
    }
}

- (Scene*)loadAssetSync:(NSString*)assetName
{
    NSData* data = [HouyiUtils getDataFromFile:assetName];
    
    string filePath = [assetName cStringUsingEncoding:NSUTF8StringEncoding];
    Loader* loader = Loader::getLoader(filePath);
    return loader->load((const char*)[data bytes], [data length]);
}

@end
