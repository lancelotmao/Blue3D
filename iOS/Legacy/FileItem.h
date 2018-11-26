//
//  FileItem.h
//  Houyi
//
//  Created by Lance Mao on 7/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <DropboxSDK/DropboxSDK.h>

@interface FileItem : NSObject
{
}

@property (retain, nonatomic) DBMetadata* dropBoxFile;
@property (retain, nonatomic) NSString* localPath;
@property (retain, nonatomic) NSString* fileName;
@property (retain, nonatomic) UIImage* thumbnail;
@property (readwrite, nonatomic) long long rev;
@property (readwrite, nonatomic) int size;
@property (readwrite, nonatomic) int thumbrev;
@property (readwrite, nonatomic) int flag;
@property (readwrite, nonatomic) bool isSample;

-(BOOL) hasTexture;
-(void) setHasTexture:(BOOL)has;

-(BOOL) hasAnimation;
-(void) setHasAnimation:(BOOL)has;

-(BOOL)textureAllLoaded;
-(void)setTextureAllLoaded:(BOOL)loaded;

+ (bool)createFolder: (NSString*)path;
+ (NSString*)getPath: (NSString*)path1 :(NSString*)path2; // replace ../
+ (NSString*)getLocalPath: (NSString*)path1 :(NSString*)path2; // replace ../ and use only file name
+ (NSString*)getThumbnailPath: (NSString*)filePath;
+ (NSString*)getHouyiPath :(NSString*)filePath;

@end
