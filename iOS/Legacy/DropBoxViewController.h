//
//  DropBoxViewController.h
//  Houyi
//
//  Created by Lance Mao on 1/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <DropboxSDK/DropboxSDK.h>
#import "FileBrowseViewController.h"

#include <Houyi/Houyi.h>

@interface DropBoxViewController : FileBrowseViewController<DBRestClientDelegate, UIDocumentInteractionControllerDelegate>
{
    IBOutlet UIButton* btnUp;
    IBOutlet UILabel* folderTitle;
    IBOutlet UIBarButtonItem* navItemGoUp;
    IBOutlet UIBarButtonItem* navItemLogOut;
    
    UIProgressView* mProgress;
    DBRestClient *restClient;
    
    FileItem* mCurItem;
    FileItem* mDownloadingItem;
    int mDownloadCount;
    
    TextureImageChecker* mTextureChecker;
    bool mNavAway;
    float mCurProgress;
    bool resetTableToFirst;
    
    UIImage* mImgIcon;
    UIImage* mImgFolder;
}

@property (nonatomic, readonly) DBRestClient *restClient;

- (void)listFolder;

@end
