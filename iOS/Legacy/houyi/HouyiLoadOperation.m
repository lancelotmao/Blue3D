//
//  HouyiLoadModelOperation.m
//  Houyi
//
//  Created by Lance Mao on 21/5/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiLoadOperation.h"
#import "FileItem.h"
#include <Houyi/Houyi.h>

@implementation LoadOperation

@synthesize index, fileName;

- (void)main
{
    NSData* data = [HouyiUtils getDataFromFile:fileName];
    if (!data) {
        return;
    }
    
    string filePath = [fileName cStringUsingEncoding:NSUTF8StringEncoding];
    loader = Loader::getLoader(filePath);
    if (!loader) {
        return;
    }
    
    Scene* scene = loader->load((const char*)[data bytes], [data length]);
    if (self.isCancelled) {
        HouyiDelete(scene);
    } else if (self.delegate) {
        if (!StringUtils::endsWith(filePath, ".houyi")) {
            string str = filePath + ".houyi";
            bool res = Saver::getSaver(str)->save(scene);
            if (!res)
            {
                LOGE("Cannot save file: %s\n", str.c_str());
            }
        }
        [self.delegate onLoadFinished:self :scene];
    }
    HouyiDelete(loader);
    loader = 0;
}

- (void)cancel
{
    if (loader) {
        loader->cancel();
    }
    [super cancel];
}

- (float)getProgress
{
    return loader ? loader->getProgress() : 0;
}

@end
