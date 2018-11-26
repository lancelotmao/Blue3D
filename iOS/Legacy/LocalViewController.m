//
//  LocalViewController.m
//  Houyi
//
//  Created by Lance Mao on 4/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "LocalViewController.h"
#import "HouyiAppDelegate.h"
#import "DataManager.h"

@implementation LocalViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	
    mSelectedItem = [NSMutableArray new];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    mIsSelectionMode = false;
    if ([mDelegate isPro]) {
        mPlaceHolder = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
        mToolbar.items = [NSArray arrayWithObjects:mPlaceHolder, mBarItemSelect, nil];
    } else {
        mToolbar.items = [NSArray arrayWithObjects:nil];
    }
    [self loadFromDB];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)loadFromDB
{
    mFileData = [mDBMan loadLocal];
    int fileCnt = [mFileData count];
    [DataManager getInstance].itemCount = fileCnt;
    [[DataManager getInstance].items removeAllObjects];
    NSString* file = NSLocalizedString((fileCnt > 1 ? @"Files" : @"File"), nil);
    int size = 0;
    for (int i = 0;i < fileCnt;++i) {
        FileItem* item = [mFileData objectAtIndex:i];
        size += item.size;
        [[DataManager getInstance].items addObject:item];
    }
    infoTitle.text = [NSString stringWithFormat:@"%d %@, %@", fileCnt, file, [self formatSize:size]];
    [mCollectionView reloadData];
}

-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView
                 cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewCell *cell = [collectionView
                                  dequeueReusableCellWithReuseIdentifier:@"FileCell"
                                  forIndexPath:indexPath];
    int row = [indexPath row];
    FileItem* item = [mFileData objectAtIndex:row];
    UIImage* thumbnail = item.thumbnail;
    
    UIImageView* iv = (UIImageView*)[cell viewWithTag:100];
    iv.image = thumbnail ? thumbnail : [UIImage imageNamed:@"icon"];
    
    UILabel* label = (UILabel*)[cell viewWithTag:101];
    label.text = [item.localPath lastPathComponent];
    
    UIImageView* ivPlay = (UIImageView*)[cell viewWithTag:102];
    ivPlay.hidden = ![item hasAnimation];
    
    UIImageView* ivFrame = (UIImageView*)[cell viewWithTag:103];
    ivFrame.hidden = YES;
    
    UIImageView* ivSelect = (UIImageView*)[cell viewWithTag:105];
    if (mIsSelectionMode && [mSelectedItem containsObject:item]) {
        ivSelect.hidden = NO;
    } else {
        ivSelect.hidden = YES;
    }
    
    return cell;
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    if ([mLoadingIndicator isAnimating]) {
        return;
    }
    
    int row = [indexPath row];
    FileItem* item = [mFileData objectAtIndex:row];
    if (mIsSelectionMode) {
        if ([mSelectedItem containsObject:item]) {
            [mSelectedItem removeObject:item];
        } else {
            [mSelectedItem addObject:item];
        }
        [mCollectionView reloadData];
        
        if ([mSelectedItem count] > 0) {
            mToolbar.items = [NSArray arrayWithObjects:mPlaceHolder, mBarItemTrash, mBarItemSelect, nil];
        } else {
            mToolbar.items = [NSArray arrayWithObjects:mPlaceHolder, mBarItemSelect, nil];
        }
        
    } else {
        [DataManager getInstance].focus = row;
        [self startViewer:item];
    }
}

- (IBAction)selectClicked:(id) sender {
    if (mIsSelectionMode) {
        [self quitSelectionMode];
    } else {
        [self enterSelectionMode];
    }
}

- (IBAction)trashClicked:(id) sender
{
    UIAlertView* _alert = [[UIAlertView alloc] init];
    [_alert setTitle:@"Confirm"];
    [_alert setMessage:@"Delete selected files?"];
    [_alert setDelegate:self];
    [_alert addButtonWithTitle:@"Yes"];
    [_alert addButtonWithTitle:@"No"];
    [_alert show];
    
}
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 0) {
        [mDBMan deleteLocal:mSelectedItem];
        [self loadFromDB];
        [self quitSelectionMode];
        [self deleteFiles];
    }
}

- (void)enterSelectionMode
{
    mIsSelectionMode = true;
    [mSelectedItem removeAllObjects];
    mBarItemSelect.title = @"Cancel";
    [mCollectionView reloadData];
}

- (void)quitSelectionMode
{
    mIsSelectionMode = false;
    mBarItemSelect.title = @"Select";
    mToolbar.items = [NSArray arrayWithObjects:mPlaceHolder, mBarItemSelect, nil];
    [mCollectionView reloadData];
}

- (void)deleteFiles
{
    for (int i = 0;i < [mSelectedItem count];++i)
    {
        FileItem* item = mSelectedItem[i];
        [self deleteFile:item.localPath];
        NSString* cacheFile = [item.localPath stringByAppendingString:@".houyi"];
        [self deleteFile:cacheFile];
    }
}

- (void)deleteFile:(NSString*)path
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error;
    BOOL success = [fileManager removeItemAtPath:path error:&error];
    if (!success) {
        NSLog(@"Could not delete file -:%@ ",[error localizedDescription]);
    }
}

@end
