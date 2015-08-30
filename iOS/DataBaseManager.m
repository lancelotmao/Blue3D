//
//  DataBaseManager.m
//  Houyi
//
//  Created by Lance Mao on 7/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "DataBaseManager.h"

@implementation DataBaseManager

static DataBaseManager *sharedInstance = nil;
static sqlite3 *database = nil;
static sqlite3_stmt *statement = nil;

+(DataBaseManager*)getSharedInstance{
    if (!sharedInstance) {
        sharedInstance = [[super allocWithZone:NULL]init];
        [sharedInstance createDB];
    }
    return sharedInstance;
}

-(BOOL)createDB{
    NSString *docsDir;
    NSArray *dirPaths;
    // Get the documents directory
    dirPaths = NSSearchPathForDirectoriesInDomains
    (NSDocumentDirectory, NSUserDomainMask, YES);
    docsDir = dirPaths[0];
    // Build the path to the database file
    databasePath = [[NSString alloc] initWithString:
                    [docsDir stringByAppendingPathComponent: @"data.db"]];
    BOOL isSuccess = YES;
    NSFileManager *filemgr = [NSFileManager defaultManager];
    if ([filemgr fileExistsAtPath: databasePath ] == NO)
    {
        const char *dbpath = [databasePath UTF8String];
        if (sqlite3_open(dbpath, &database) == SQLITE_OK)
        {
            char *errMsg;
            const char *sql_stmt =
            "create table if not exists file_table (filepath text, revision integer, size integer, thumbrev integer, flag integer)";
            if (sqlite3_exec(database, sql_stmt, NULL, NULL, &errMsg)
                != SQLITE_OK)
            {
                isSuccess = NO;
                NSLog(@"Failed to create table");
            }
            sqlite3_close(database);
            return  isSuccess;
        }
        else {
            isSuccess = NO;
            NSLog(@"Failed to open/create database");
        }
    }
    return isSuccess;
}

-(BOOL)insertFile:(FileItem*) item
{
    FileItem* exist = [self findByFilePath:item.localPath];
    if (exist) {
        NSLog(@"insertFile. item already exist database path = %@", item.localPath);
        return NO;
    }
    
    [mLock lock];
    BOOL res = NO;
    NSString* filePath = item.localPath;
    int revision = item.rev;
    int size = item.size;
    // first time insert thumbnail rev same as file
    // but this doesn't mean thumbnail is valid
    // because it might not exist at all
    int thumbrev = revision;
    int flag = item.flag;
    const char *dbpath = [databasePath UTF8String];
    if (sqlite3_open(dbpath, &database) == SQLITE_OK)
    {
        NSString *insertSQL = [NSString stringWithFormat:@"insert into file_table (filepath, revision, size, thumbrev, flag) values(\"%@\", \"%d\", \"%d\", \"%d\", \"%d\")", filePath, revision, size, thumbrev, flag];
        const char *insert_stmt = [insertSQL UTF8String];
        sqlite3_prepare_v2(database, insert_stmt, -1, &statement, NULL);
        if (sqlite3_step(statement) == SQLITE_DONE) {
            res = YES;
        } else {
            res = NO;
        }
        sqlite3_reset(statement);
    }
    sqlite3_close(database);
    [mLock unlock];
    return res;
}

-(FileItem*)findByFilePath:(NSString*)filePath
{
    [mLock lock];
    FileItem* item = nil;
    const char *dbpath = [databasePath UTF8String];
    if (sqlite3_open(dbpath, &database) == SQLITE_OK) {
        NSString *querySQL = [NSString stringWithFormat:
                              @"select revision, flag from file_table where filepath=\"%@\"", filePath];
        const char *query_stmt = [querySQL UTF8String];
        if (sqlite3_prepare_v2(database, query_stmt, -1, &statement, NULL) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                item = [FileItem new];
                item.localPath = filePath;
                item.rev = [[[NSString alloc] initWithUTF8String:(const char *) sqlite3_column_text(statement, 0)] intValue];
                item.flag = [[[NSString alloc] initWithUTF8String:(const char *) sqlite3_column_text(statement, 1)] intValue];
            } else {
//                NSLog(@"Item Not found in database path = %@", filePath);
            }
            sqlite3_reset(statement);
        }
    }
    sqlite3_close(database);
    [mLock unlock];
    return item;
}

-(NSMutableArray*)loadLocal
{
    [mLock lock];
    NSMutableArray* res = [NSMutableArray new];
    const char *dbpath = [databasePath UTF8String];
    if (sqlite3_open(dbpath, &database) == SQLITE_OK) {
        NSString *querySQL = [NSString stringWithFormat:@"select filepath, size, flag from file_table"];
        const char *query_stmt = [querySQL UTF8String];
        if (sqlite3_prepare_v2(database, query_stmt, -1, &statement, NULL) == SQLITE_OK) {
            while (sqlite3_step(statement) == SQLITE_ROW) {
                FileItem* item = [FileItem new];
                item.localPath = [[NSString alloc] initWithUTF8String:(const char *) sqlite3_column_text(statement, 0)];
                item.thumbnail = [UIImage imageWithContentsOfFile:[FileItem getThumbnailPath:item.localPath]];
                item.size = [[[NSString alloc] initWithUTF8String:(const char *) sqlite3_column_text(statement, 1)] intValue];
                item.flag = [[[NSString alloc] initWithUTF8String:(const char *) sqlite3_column_text(statement, 2)] intValue];
                [res addObject:item];
            }
            sqlite3_reset(statement);
        }
    }
    sqlite3_close(database);
    [mLock unlock];
    return res;
}

- (void)deleteLocal:(NSMutableArray*)items
{
    [mLock lock];
    NSString* querySQL = @"delete from file_table where filepath=";
    for (int i = 0;i < [items count];++i)
    {
        FileItem* item = [items objectAtIndex:i];
        querySQL = [querySQL stringByAppendingFormat:@"\"%@\"", item.localPath];
        if (i < [items count] - 1) {
            querySQL = [querySQL stringByAppendingString:@" or filepath="];
        }
    }
    NSLog(@"delete sql = %@", querySQL);
    const char *dbpath = [databasePath UTF8String];
    if (sqlite3_open(dbpath, &database) == SQLITE_OK) {
        const char *query_stmt = [querySQL UTF8String];
        if (sqlite3_prepare_v2(database, query_stmt, -1, &statement, NULL) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_DONE) {
                NSLog(@"file deleted. count = %d", [items count]);
            }
            sqlite3_reset(statement);
        }
    }
    sqlite3_close(database);
    [mLock unlock];
}

- (bool)thumbnailValid:(NSString*)filePath :(int)fileRev
{
    NSString* thumbnailPath = [FileItem getThumbnailPath:filePath];
    return [[NSFileManager defaultManager] fileExistsAtPath:thumbnailPath];
}

- (bool)updateThumbnail:(FileItem*) item :(bool)valid
{
    // only update thumbnail revision when download from dropbox
    // if from local thumbnail revision cannot change
    if (!item.dropBoxFile) {
        return false;
    }
    
    bool ret = false;
    [mLock lock];
    const char *dbpath = [databasePath UTF8String];
    if(sqlite3_open(dbpath, &database) == SQLITE_OK)
    {
        NSString* filePath = item.localPath;
        int filerev = valid ? item.dropBoxFile.revision : 0;
        NSString *querySQL = [NSString stringWithFormat:@"update file_table Set thumbrev = \"%d\" where filepath=\"%@\"",
                              filerev, filePath];
        const char *query_stmt = [querySQL UTF8String];
        if(sqlite3_prepare_v2(database, query_stmt, -1, &statement, NULL)==SQLITE_OK){
            if(SQLITE_DONE != sqlite3_step(statement)) {
                NSLog(@"Error while updating. '%s'", sqlite3_errmsg(database));
            } else {
                sqlite3_reset(statement);
                NSLog(@"Thumbnail rev updated. Path = %@, thumbnail version = %d", filePath, filerev);
                ret = true;
            }
        }
    }
    sqlite3_close(database);
    [mLock unlock];
    return ret;
}

@end
