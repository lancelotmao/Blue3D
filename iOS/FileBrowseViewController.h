//
//  FileBrowseViewController.h
//  Houyi
//
//  Created by Lance Mao on 4/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DataBaseManager.h"
#import "FileItem.h"
#import "iAd/ADBannerView.h"

@class HouyiAppDelegate;

@interface FileBrowseViewController : UIViewController
{
    IBOutlet UICollectionView* mCollectionView;
    ADBannerView* mBanner;
    UIActivityIndicatorView* mLoadingIndicator;
    
    HouyiAppDelegate* mDelegate;
    DataBaseManager* mDBMan;
    NSMutableArray* mFileData;
    NSMutableArray* mFolderData;
    NSString* mCD;
}

- (void)startViewer:(FileItem*)item;
- (bool) maxCountReached:(NSString*)nextRequestPath;
- (NSString*)formatSize:(long long)size;
- (void)purchased;

@end
