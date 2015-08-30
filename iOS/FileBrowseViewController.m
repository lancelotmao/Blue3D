//
//  FileBrowseViewController.m
//  Houyi
//
//  Created by Lance Mao on 4/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "FileBrowseViewController.h"
#import "HouyiAppDelegate.h"
#import "HouyiIAPManager.h"

@implementation FileBrowseViewController

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
	UINib *cellNIB = [UINib nibWithNibName:@"ItemViewCell" bundle:nil];
    [mCollectionView registerNib:cellNIB forCellWithReuseIdentifier:@"FileCell"];
    
    mDelegate = (HouyiAppDelegate*)[UIApplication sharedApplication].delegate;
    if(![mDelegate isPro]) {
        mBanner = [[ADBannerView alloc] initWithFrame:CGRectZero];
        [self.view addSubview:mBanner];
        [mBanner setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        [self updateCollectionView];
        
        [mCollectionView setTranslatesAutoresizingMaskIntoConstraints:NO];
        // pin sides to superview
        [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[mBanner]-0-|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(mBanner)]];
        
        // set height to a constant
        [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:[mBanner(==66)]" options:0 metrics:nil views:NSDictionaryOfVariableBindings(mBanner)]];
        
        // pin contentView to bannerView with 0 length constraint
        [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:[mCollectionView]-0-[mBanner]" options:0 metrics:nil views:NSDictionaryOfVariableBindings(mCollectionView,mBanner)]];
    }
    
    mDBMan = [DataBaseManager getSharedInstance];
    mFileData = [NSMutableArray new];
    mFolderData = [NSMutableArray new];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self updateCollectionView];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [self updateCollectionView];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)updateCollectionView
{
    float bannerH = 0;
    if(![mDelegate isPro]) {
        bannerH = mBanner.frame.size.height;
    }
    
    for(NSLayoutConstraint *constraint in self.view.constraints)
    {
        if(constraint.firstAttribute == NSLayoutAttributeBottom && constraint.secondAttribute == NSLayoutAttributeBottom &&
           constraint.firstItem == self.view && constraint.secondItem == mCollectionView)
        {
            constraint.constant = 50 + bannerH;
        }
    }
}

-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return [mFileData count];
}


- (void)startViewer:(FileItem*)item
{
    if (item == nil) {
        return;
    }
    
    if (item.dropBoxFile) {
        item.rev = item.dropBoxFile.revision;
        item.size = item.dropBoxFile.totalBytes;
    }
    
    item.isSample = NO;
    Blue3DViewController* vc = [mDelegate getViewerVC];
    [self.tabBarController presentViewController:vc animated:YES completion:nil];
}

- (bool) maxCountReached:(NSString*)nextRequestPath
{
    NSArray* files = [mDBMan loadLocal];
    for (int i = 0;i < [files count];++i) {
        FileItem* item = [files objectAtIndex:i];
        if ([item.localPath isEqualToString:nextRequestPath]) {
            return false;
        }
    }
    return [files count] >= [HouyiIAPManager getMaxModelCountForFree];
}

- (NSString*)formatSize:(long long)size
{
    NSString* res;
    if (size > 1024 * 1024) {
        size = size / 1024 / 1024;
        res = [NSString stringWithFormat:@"%lldM", size];
    } else if (size > 1024) {
        size = size / 1024;
        res = [NSString stringWithFormat:@"%lldK", size];
    } else {
        res = [NSString stringWithFormat:@"%lldBytes", size];
    }
    return res;
}

- (void)purchased
{
    [self cleanRemoveFromSuperview:mBanner];
    for(NSLayoutConstraint *constraint in self.view.constraints)
    {
        if(constraint.firstAttribute == NSLayoutAttributeBottom && constraint.secondAttribute == NSLayoutAttributeBottom &&
           constraint.firstItem == self.view && constraint.secondItem == mCollectionView)
        {
            constraint.constant = 50;
        }
    }
}

- (void)cleanRemoveFromSuperview:(UIView *) view
{
    if(!view || !view.superview) return;
    
    //First remove any constraints on the superview
    NSMutableArray * constraints_to_remove = [NSMutableArray new];
    UIView * superview = view.superview;
    
    for( NSLayoutConstraint * constraint in superview.constraints) {
        if( constraint.firstItem == view ||constraint.secondItem == view ) {
            [constraints_to_remove addObject:constraint];
        }
    }
    [superview removeConstraints:constraints_to_remove];
    
    //Then remove the view itself.
    [view removeFromSuperview];
}

@end
