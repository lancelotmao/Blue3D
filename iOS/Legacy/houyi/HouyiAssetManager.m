//
//  HouyiAssetManager.m
//  Houyi
//
//  Created by Lance Mao on 17/4/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiAssetManager.h"
#import "HouyiViewController.h"

@interface HouyiAssetManager () {
    NSMutableDictionary* _loadingOps;
}

@end

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
        _loadingOps = [NSMutableDictionary new];
    }
    return self;
}

- (void)cancelAll
{
    [_loadingOps removeAllObjects];
    [queue cancelAllOperations];
}

- (void)cancelInActive :(NSRange)activeRange
{
    NSInteger start = activeRange.location;
    NSInteger length = activeRange.length;
    for (NSString* key in _loadingOps) {
        LoadOperation* op = [_loadingOps objectForKey:key];
        if (op.index < start || op.index >= start + length) {
            [_loadingOps removeObjectForKey:op.fileName];
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
    return [_loadingOps objectForKey:path];
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

    if (!_loadingOps[assetName]) {
        LoadOperation *op = [LoadOperation new];
        op.fileName = assetName;
        op.index = index;
        op.delegate = self;
        [queue addOperation:op];
        [_loadingOps setObject:op forKey:assetName];
    }
}

- (Scene*)loadAssetSync:(NSString*)assetName
{
    NSData* data = [HouyiUtils getDataFromFile:assetName];
    
    string filePath = [assetName cStringUsingEncoding:NSUTF8StringEncoding];
    Loader* loader = Loader::getLoader(filePath);
    return loader->load((const char*)[data bytes], (int)[data length]);
}

- (void)onLoadFinished:(LoadOperation*)op :(Scene*)scene {
    [_loadingOps removeObjectForKey:op.fileName];
    [self.loadDelegate onLoadFinished:op :scene];
}

@end
