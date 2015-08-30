//
//  FileItem.m
//  Houyi
//
//  Created by Lance Mao on 7/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "FileItem.h"

@implementation FileItem

@synthesize dropBoxFile, localPath, fileName, thumbnail, rev, size, thumbrev, flag, isSample;

int TEXTURE_BIT = 1;
int ANIMATION_BIT = 1 << 1;
int TEXTURE_ALL_LOADED_BIT = 1 << 2;

-(BOOL) hasTexture
{
    return (flag & TEXTURE_BIT) != 0;
}

-(void) setHasTexture:(BOOL)has
{
    if (has) {
        flag |= TEXTURE_BIT;
    } else {
        flag &= ~TEXTURE_BIT;
    }
}

-(BOOL) hasAnimation
{
    return (flag & ANIMATION_BIT) != 0;
}

-(void) setHasAnimation:(BOOL)has
{
    if (has) {
        flag |= ANIMATION_BIT;
    } else {
        flag &= ~ANIMATION_BIT;
    }
}

-(BOOL)textureAllLoaded
{
    return( flag & TEXTURE_ALL_LOADED_BIT) != 0;
}

-(void)setTextureAllLoaded:(BOOL)loaded
{
    if (loaded) {
        flag |= TEXTURE_ALL_LOADED_BIT;
    } else {
        flag &= ~TEXTURE_ALL_LOADED_BIT;
    }
}

+ (bool)createFolder: (NSString*)path
{
    NSError *error;
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        if (![[NSFileManager defaultManager] createDirectoryAtPath:path
                                       withIntermediateDirectories:YES
                                                        attributes:nil
                                                             error:&error]) {
            NSLog(@"Create directory error: %@", error);
            return false;
        }
    }
    return true;
}

+ (NSString*)getPath: (NSString*)path1 :(NSString*)path2
{
    while ([path2 hasPrefix:@"../"]) {
        path1 = [[path1 stringByDeletingLastPathComponent] stringByAppendingString:@"/"];
        path2 = [path2 substringFromIndex:3];
    }
    while ([path2 hasPrefix:@"./"]) {
        path1 = [[path1 stringByDeletingLastPathComponent] stringByAppendingString:@"/"];
        path2 = [path2 substringFromIndex:2];
    }
    return [path1 stringByAppendingString:path2];
}

+ (NSString*)getLocalPath: (NSString*)path1 :(NSString*)path2
{
    while ([path2 hasPrefix:@"../"]) {
        path1 = [[path1 stringByDeletingLastPathComponent] stringByAppendingString:@"/"];
        path2 = [path2 substringFromIndex:3];
    }
    while ([path2 hasPrefix:@"./"]) {
        path1 = [[path1 stringByDeletingLastPathComponent] stringByAppendingString:@"/"];
        path2 = [path2 substringFromIndex:2];
    }
    return [path1 stringByAppendingString:[path2 lastPathComponent]];
}

+ (NSString*)getThumbnailPath: (NSString*)filePath
{
    return [filePath stringByAppendingString:@"_.jpg"];
}

+ (NSString*)getHouyiPath :(NSString*)filePath
{
    return [filePath stringByAppendingString:@".houyi"];
}
@end
