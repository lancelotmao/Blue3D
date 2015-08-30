//
//  DataBaseManager.h
//  Houyi
//
//  Created by Lance Mao on 7/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <sqlite3.h>

#import "FileItem.h"

@interface DataBaseManager : NSObject
{
    NSString *databasePath;
    NSLock* mLock;
}

+(DataBaseManager*)getSharedInstance;
-(BOOL)createDB;
-(BOOL)insertFile:(FileItem*) item;
-(FileItem*)findByFilePath:(NSString*)filePath;
-(NSMutableArray*)loadLocal;
- (void)deleteLocal:(NSMutableArray*)items;
- (bool)thumbnailValid:(NSString*)filePath :(int)fileRev;
- (bool)updateThumbnail:(FileItem*) item :(bool)valid;

@end
