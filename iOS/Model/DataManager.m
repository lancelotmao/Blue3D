//
//  DataManager.m
//  Houyi
//
//  Created by Lance Mao on 14/5/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "DataManager.h"
#import "FileItem.h"
#import "HouyiAssetManager.h"
//#import "BlueSetting.h"
//#import "HouyiAppDelegate.h"
#include <Houyi/Houyi.h>

using namespace Houyi;

@implementation DataManager

@synthesize itemCount, focus, cacheSize, items, focusDelegate;

static DataManager* mIns = 0;

+(DataManager*)getInstance
{
    if (!mIns) {
        mIns = [DataManager new];
        mIns.items = [NSMutableArray new];
        mIns.cacheSize = 5;
    }
    return mIns;
}

-(void)changeFocus:(int)newFocus
{
    if (focus != newFocus) {
        NSLog(@"DataManager focus changed. old: %d, new %d", focus, newFocus);
//        World* world = [HouyiAppDelegate getRoot]->getWorld();
//        world->clearScene();
        FileItem* item = [items objectAtIndex:newFocus];
        if (!item.localPath) {
            NSLog(@"cannot change focus item path is NULL");
            return;
        }
        string filePath = [item.localPath cStringUsingEncoding:NSUTF8StringEncoding];
        Scene* scene = SceneManager::getInstance()->findSceneByName(filePath);

        // first delete out of range scenes to free up memory before loading
        [self deleteOutOfRangeScene :newFocus];

        // cancel loading so that thread pool can free up resource for current loading
        NSRange range = [self getRange :focus :[items count]];
        HouyiAssetManager* assetMan = [HouyiAssetManager getInstance :self];
        [assetMan cancelInActive:range];

        if (!scene) {
            [focusDelegate onFocusChanged:focus :newFocus];
            [assetMan loadAsset:item.localPath focusIndex:newFocus];
        } else {
            [focusDelegate onFocusModelLoaded:newFocus :scene];
        }

        focus = newFocus;
    }
}

- (float)getFocusProgress
{
    HouyiAssetManager* assetMan = [HouyiAssetManager getInstance :self];
    FileItem* item = [self getFocusItem];
    LoadOperation* op = [assetMan getOperation :item.localPath];
    return [op getProgress];
}

- (void)onLoadFinished:(LoadOperation*)op :(Scene*)scene;
{
    NSLog(@"Load Model Operation completed. Index = %d, fileName = %@", op.index, op.fileName);
    
    if (scene == nil) {
        NSLog(@"nil scene returned from operation: %@", op.fileName);
        return;
    }
    
    int index = op.index;
    NSRange range = [self getRange :focus :[items count]];
    if (index < range.location || index > range.location + range.length) {
        NSLog(@"too far from focus, discard: %s", scene->getName().c_str());
        HouyiDelete(scene);
        return;
    }
    
    SceneManager::getInstance()->addScene(scene);
    if(op.index == focus) {
        [focusDelegate onFocusModelLoaded:focus :scene];
    }
    
    [self addPrefetchTask];
}

- (NSRange)getRange :(int)middle :(int)count
{
    NSRange ret;
    ret.length = min(cacheSize, count);
    int length = cacheSize / 2;
    if (middle >= length && middle < count - length) {
        ret.location = middle - length;
    } else if (middle < length) {
        ret.location = 0;
    } else {
        ret.location = count - ret.length;
    }
    
    return ret;
}

- (void)deleteOutOfRangeScene :(int)newFocus
{
    SceneManager* sm = SceneManager::getInstance();
    int start = newFocus - cacheSize / 2;
    if (start < 0) start = 0;
    for (int i = sm->getSceneCount() - 1;i >= 0;--i)
    {
        Scene* scene = sm->getScene(i);
        for (int j = 0;j < start;++j)
        {
            FileItem* item = items[j];
            if (scene->getName().compare([item.localPath UTF8String]) == 0)
            {
                sm->deleteSceneDefer(i);
            }
        }
        
        for (int j = start + cacheSize;j < [items count];++j)
        {
            FileItem* item = items[j];
            if (scene->getName().compare([item.localPath UTF8String]) == 0)
            {
                sm->deleteSceneDefer(i);
            }
        }
    }
}

- (void)addPrefetchTask
{
    dispatch_async( dispatch_get_main_queue(), ^{
    
        NSRange range = [self getRange :focus :[items count]];
        for (int i = 0;i < range.length;++i) {
            int index = i + range.location;
            FileItem* item = items[index];
            if (item.localPath == nil) {
                continue;
            }
            string filePath = [item.localPath cStringUsingEncoding:NSUTF8StringEncoding];
            Scene* scene = SceneManager::getInstance()->findSceneByName(filePath);
            if (!scene) {
                HouyiAssetManager* assetMan = [HouyiAssetManager getInstance :self];
                [assetMan loadAsset:item.localPath focusIndex:index];
            }
        }
    });
}

- (id)getFocusItem
{
    if (focus >= 0 && focus < [items count])
        return items[focus];
    return 0;
}

@end
