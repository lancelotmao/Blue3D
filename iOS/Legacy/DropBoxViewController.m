//
//  DropBoxViewController.m
//  Houyi
//
//  Created by Lance Mao on 1/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiAppDelegate.h"
#import "DropBoxViewController.h"
#import "HouyiIAPManager.h"
#import "DataManager.h"
#import <DropboxSDK/DropboxSDK.h>

@implementation DropBoxViewController

@synthesize restClient;

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
    
	mDelegate.mDBVC = self;
    
    mCD = @"/";
    mCurItem = [FileItem new];
    
    mImgIcon = [UIImage imageNamed:@"icon"];
    mImgFolder = [UIImage imageNamed:@"folder"];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    mNavAway = NO;
    if (mDownloadingItem != nil) {
        [self showLoading];
        [mCollectionView reloadData];
    } else if (![[DBSession sharedSession] isLinked]) {
        [[DBSession sharedSession] linkFromController:self];
    } else {
        [self listFolder];
    }
    
    [self updateNavButton];
}

- (void)viewWillDisappear:(BOOL)animated
{
    mNavAway = YES;
    [self dismissLoading];
//    mDownloadingItem = nil;
    [mCollectionView reloadData];
    self.tabBarController.navigationItem.rightBarButtonItem = nil;
}

- (void)listFolder
{
    [self updateNavButton];
    [self showLoading];
    [[self restClient] loadMetadata:mCD];
}

- (DBRestClient *)restClient {
    if (!restClient) {
        restClient =
        [[DBRestClient alloc] initWithSession:[DBSession sharedSession]];
        restClient.delegate = self;
    }
    return restClient;
}

- (void)restClient:(DBRestClient *)client loadedMetadata:(DBMetadata *)metadata {
    [self dismissLoading];
    if (metadata.isDirectory) {
        DataManager* dataMan = [DataManager getInstance];
        [dataMan.items removeAllObjects];
        [mFileData removeAllObjects];
//        NSLog(@"Folder '%@' contains:", metadata.path);
        for (DBMetadata *file in metadata.contents) {
//            NSLog(@"	%@", file.filename);
            if (file.isDirectory || [self isSupportedFile:file.filename]) {
                FileItem* item = [FileItem new];
                item.dropBoxFile = file;
                [mFileData addObject:item];
                if(file.isDirectory) {
                    [mFolderData addObject:item];
                } else {
                    [dataMan.items addObject:item];
                }
            }
        }

        [mCollectionView reloadData];
        if (resetTableToFirst) {
            [mCollectionView scrollRectToVisible:CGRectMake(0, 0, 1, 1) animated:NO];
            resetTableToFirst = NO;
        }
        if ([mCD isEqualToString:@"/"]) { // root
            folderTitle.text = NSLocalizedString(@"DropBox Root", nil);
        } else {
            folderTitle.text = [mCD lastPathComponent];
        }
    }
}

- (void)restClient:(DBRestClient *)client loadMetadataFailedWithError:(NSError *)error {
    NSLog(@"Error loading metadata: %@", error);
    [self dismissLoading];
}

-(IBAction) logout:(id) sender
{
    NSLog(@"Logging out Dropbox");
    [[DBSession sharedSession] unlinkAll];
    [mFileData removeAllObjects];
    [mCollectionView reloadData];
    btnUp.hidden = YES;
    folderTitle.text = nil;
    mDownloadingItem = nil;
    mDownloadCount = 0;
    mCD = @"/";
    [self dismissLoading];
    [[DBSession sharedSession] linkFromController:self];
    [self updateNavButton];
}

- (void)updateNavButton
{
    if ([[DBSession sharedSession] isLinked]) {
        navItemLogOut.enabled = YES;
        navItemLogOut.title = NSLocalizedString(@"Log Out", nil);
        navItemLogOut.width = 0;
    } else {
        navItemLogOut.enabled = NO;
        navItemLogOut.title = nil;
        navItemLogOut.width = 0.01;
    }
    
    navItemGoUp.title = NSLocalizedString(@"Go Up", nil);
}

-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView
                 cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewCell *cell = [collectionView
                                  dequeueReusableCellWithReuseIdentifier:@"FileCell"
                                  forIndexPath:indexPath];
    int row = [indexPath row];
    FileItem* item = [mFileData objectAtIndex:row];
    DBMetadata* file = item.dropBoxFile;
    
    UIImage *image = nil;
    if (file.isDirectory) {
        image = mImgFolder;
    } else if ([self isModelFile:file.filename]) {
        NSString* localFilePath = [self getLocalFilePath:item];
        NSString* thumbnailPath = [FileItem getThumbnailPath:localFilePath];
        int rev = file.revision;
        if([mDBMan thumbnailValid:localFilePath : rev]) {
            image = [UIImage imageWithContentsOfFile:thumbnailPath];
            item.thumbnail = image;
        }
        if (image == nil) {
            image = mImgIcon;
        }
    }
    NSString* name = file.filename;
    
    UIImageView* iv = (UIImageView*)[cell viewWithTag:100];
    iv.image = image;
    
    UILabel* label = (UILabel*)[cell viewWithTag:101];
    if (file.isDirectory) {
        label.text = name;
    } else {
        label.text = [NSString stringWithFormat:@"%@ %@", name, [self formatSize:file.totalBytes]];
    }
    
    UIImageView* ivPlay = (UIImageView*)[cell viewWithTag:102];
    ivPlay.hidden = ![item hasAnimation];
    
    UIImageView* ivFrame = (UIImageView*)[cell viewWithTag:103];
    ivFrame.hidden = YES;
    
    UIProgressView* p = (UIProgressView*)[cell viewWithTag:104];
    if (mDownloadingItem && [mDownloadingItem.dropBoxFile.filename isEqualToString:item.dropBoxFile.filename]) {
        mProgress = p;
        mProgress.hidden = NO;
        [mProgress setProgress:mCurProgress animated:NO];
        if (mDownloadCount == 0)
            label.text = mDownloadingItem.fileName;
        else
            label.text = NSLocalizedString(@"Downloading Texture", nil);
    } else {
        p.hidden = YES;
        label.hidden = NO;
    }
    
    return cell;
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    if ([mLoadingIndicator isAnimating]) {
        return;
    }
    
    mCurItem.dropBoxFile = nil;
    
    int row = [indexPath row];
    FileItem* item = [mFileData objectAtIndex:row];
    DBMetadata* file = item.dropBoxFile;
    if (file.isDirectory) {
        if (![mCD hasSuffix:@"/"]) {
            mCD = [mCD stringByAppendingString:@"/"];
        }
        mCD = [NSString stringWithFormat:@"%@%@/", mCD, file.filename];
        resetTableToFirst = YES;
        [self listFolder];
    } else {
        NSString* filePath = [NSString stringWithFormat:@"%@%@", mCD, item.dropBoxFile.filename];
        NSString *localPath = [self getLocalPath];
        NSString* localFilePath = [self getLocalFilePath:item];
        
        HouyiAppDelegate* delegate = (HouyiAppDelegate*)[UIApplication sharedApplication].delegate;
        // check for free user
        if (![delegate isPro] && [self maxCountReached:localFilePath]) {
            NSLog(@"Max File Count reached, start IAP");
            [[HouyiIAPManager getInstance] requestProUpgradeProductData];
            // TODO show dialog when product is valid
            [[HouyiIAPManager getInstance] showProductDialog];
            return;
        }
        
        DataManager* dataMan = [DataManager getInstance];
        for (int i = 0;i < [dataMan.items count]; ++i) {
            FileItem* dItem = [dataMan.items objectAtIndex:i];
            if ([item.dropBoxFile.path isEqualToString:dItem.dropBoxFile.path]) {
                dataMan.focus = i;
                break;
            }
        }
        
        [FileItem createFolder:localPath];
        
        mCurItem = [dataMan.items objectAtIndex:dataMan.focus];
        mCurItem.localPath = localFilePath;
        mCurItem.dropBoxFile = file;
        mCurItem.rev = file.revision;
        long long dbRev = file.revision;
        FileItem* localItem = [mDBMan findByFilePath:localFilePath];
        if (dbRev > localItem.rev) {
            // only download when new version is found
            mDownloadingItem = item;
            mDownloadingItem.localPath = localFilePath;
            mDownloadingItem.fileName = file.filename;
            [mCollectionView reloadData];
            [self showLoading];
            [[self restClient] loadFile:filePath intoPath:localFilePath];
        } else {
            if ([localItem hasTexture] && ![localItem textureAllLoaded]) {
                [self prepareModelFile:localFilePath];
                mDownloadingItem = item;
                mDownloadingItem.localPath = localFilePath;
                [self showLoading];
            } else {
                [self startViewer:mCurItem];
            }
        }
    }
}

- (void)restClient:(DBRestClient*)client loadedFile:(NSString*)localPath
       contentType:(NSString*)contentType metadata:(DBMetadata*)metadata {
    NSLog(@"File loaded into path: %@", localPath);

    if ([self isSupportedFile:localPath]) {
        BOOL valid = [self prepareModelFile:localPath];
        if (!valid) {
            NSLog(@"invalid dae file downloaded from dropbox. Abort");
            mDownloadingItem = nil;
            [mCollectionView reloadData];
        } else {
            mCurItem.dropBoxFile = metadata;
            if ([mCurItem hasTexture]) {
                // set true initialially when load failed, set false
                [mCurItem setTextureAllLoaded:YES];
                mCurItem.localPath = localPath;
            } else {
                [self startViewer:mCurItem];
            }
        }
    } else if (mCurItem != nil){
        mDownloadCount--;
        if (mDownloadCount <= 0) {
            [mDBMan updateThumbnail:mCurItem:NO];
            [self startViewer:mCurItem];
        }
    } else {
        NSURL* url = [NSURL fileURLWithPath:localPath];
        UIDocumentInteractionController* docController = [UIDocumentInteractionController interactionControllerWithURL:url];
        docController.delegate = self;
        [docController presentPreviewAnimated:YES];
    }
    
    if (mDownloadCount <= 0) {
        [self dismissLoading];
    }
}

- (void)restClient:(DBRestClient*)client loadFileFailedWithError:(NSError*)error {
    NSLog(@"There was an error loading the file - %@", error);
    mDownloadCount--;
    [mCurItem setTextureAllLoaded:NO];
    if (mDownloadCount == 0) { // might be -1 if dae download failed
        [self startViewer:mCurItem];
        [mLoadingIndicator stopAnimating];
        [mLoadingIndicator removeFromSuperview];
    } else if (mDownloadCount < 0) {
        [mLoadingIndicator stopAnimating];
        [mLoadingIndicator removeFromSuperview];
        mDownloadingItem = nil;
        mDownloadCount = 0;
        [mCollectionView reloadData];
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Error"
                                            message:@"File download failed"
                                           delegate:nil
                                  cancelButtonTitle:NSLocalizedString(@"OK", @"")
                                  otherButtonTitles:nil];
        [alert show];
    }
}

- (void)restClient:(DBRestClient*)client loadProgress:(CGFloat)progress forFile:(NSString *)destPath
{
    mCurProgress = progress;
    [mCollectionView reloadData];
}

- (UIViewController *) documentInteractionControllerViewControllerForPreview: (UIDocumentInteractionController *) controller {
    return self;
}

-(IBAction) goUp:(id) sender {
    if (![mLoadingIndicator isAnimating] && ![mCD isEqualToString:@"/"]
        && mDownloadingItem == nil && [[DBSession sharedSession] isLinked]) {
        mCD = [mCD stringByDeletingLastPathComponent];
        if (![mCD hasSuffix:@"/"]) {
            mCD = [mCD stringByAppendingString:@"/"];
        }
        resetTableToFirst = YES;
        [self listFolder];
    }
}

- (void)showLoading
{
    if (mLoadingIndicator != nil) {
        [self dismissLoading];
    }
    
    mLoadingIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    [mLoadingIndicator setColor:[UIColor whiteColor]];
    mLoadingIndicator.center=self.view.center;
    [self.view addSubview:mLoadingIndicator];
    [mLoadingIndicator startAnimating];
}

- (void)dismissLoading
{
    [mLoadingIndicator stopAnimating];
    [mLoadingIndicator removeFromSuperview];
    mLoadingIndicator = nil;
}

- (BOOL)prepareModelFile : (NSString*) filePath
{
    mDownloadCount = 0;
    
    if ([filePath hasSuffix:@"dae"]) {
        TextureImageChecker checker;
        NSData* data = [HouyiUtils getDataFromFile:filePath];
        Scene* s = checker.loadFromMemory((const char*)[data bytes], [data length]);
        if (!s) {
            NSLog(@"Invalid collada file. filePath = %@", filePath);
            return NO;
        }
        
        FileItem* item = mCurItem;
        
        NSString* path = [filePath stringByDeletingLastPathComponent];
        if (![path hasSuffix:@"/"]) {
            path = [path stringByAppendingString:@"/"];
        }
        for (int i = 0;i < checker.mCollada.mImages.size();++i)
        {
            collada::Image img = checker.mCollada.mImages[i];
            NSString* imgPath = [NSString stringWithUTF8String:img.mInitFrom.c_str()];
            imgPath = [imgPath stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];// for %20 etc
            NSString* localImgPathFull = [FileItem getPath:path :imgPath];
            [FileItem createFolder:[localImgPathFull stringByDeletingLastPathComponent]];
            NSLog(@"Found texture image full path = %@", localImgPathFull);
            
            NSString* dbFilePath =  [FileItem getPath:mCD :imgPath];
            [[self restClient] loadFile:dbFilePath intoPath:localImgPathFull];
            mDownloadCount++;
            
            NSString* imgName = [imgPath lastPathComponent];
            if (![imgPath isEqualToString:imgName]) {
                NSString* localImgPathWithOnlyName = [path stringByAppendingFormat:@"/%@", imgName];   
                NSLog(@"Found texture image with only name = %@", localImgPathWithOnlyName);
                NSString* dbFilePath = [NSString stringWithFormat:@"%@%@", mCD, imgName];
                [[self restClient] loadFile:dbFilePath intoPath:localImgPathWithOnlyName];
                mDownloadCount++;
            }
        }
        
        if (mDownloadCount > 0)
        {
            [mCollectionView reloadData];
        }
        
        [item setHasTexture:checker.mCollada.mImages.size() > 0];
        [item setHasAnimation:s->getSkeletonCount()];
        
        HouyiDelete(s);
    }
    return YES;
}

- (bool)isSupportedFile:(NSString*)fileName
{
    return [self isModelFile:fileName]
    || [[fileName lowercaseString] hasSuffix:@"tmx"];
    // || [self isTextureFile:fileName];
}

- (bool)isModelFile:(NSString*)fileName
{
    return [[fileName lowercaseString] hasSuffix:@"dae"]
    || [[fileName lowercaseString] hasSuffix:@"obj"]
    || [[fileName lowercaseString] hasSuffix:@"3ds"]
    || [[fileName lowercaseString] hasSuffix:@"stl"];
}

- (bool)isTextureFile:(NSString*)fileName
{
    return [[fileName lowercaseString] hasSuffix:@"png"]
    || [[fileName lowercaseString] hasSuffix:@"jpg"]
    || [[fileName lowercaseString] hasSuffix:@"jpeg"];
}

- (NSString*)dbPathToLocalPath:(NSString*)db
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *localPath = [[documentsDirectory stringByAppendingPathComponent:@"dropbox"] stringByAppendingPathComponent:db];
    return localPath;
}

- (void)startViewer:(FileItem*)item
{
    if (item.dropBoxFile) {
        item.rev = item.dropBoxFile.revision;
        item.size = item.dropBoxFile.totalBytes;
    }
    [mDBMan insertFile:item];
    mDownloadingItem = nil;
    mCurProgress = 0;
    if (!mNavAway) {
        [super startViewer:item];
//        [DataManager getInstance].itemCount = [mFileData count] - [mFolderData count];
        [DataManager getInstance].itemCount = 0;
    }
}

- (NSString*) getLocalPath
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *localPath = [[documentsDirectory stringByAppendingPathComponent:@"dropbox"] stringByAppendingPathComponent:mCD];
    return localPath;
}

- (NSString*) getLocalFilePath:(FileItem*)item
{
    NSString *localPath = [self getLocalPath];
    NSString *localFilePath = [localPath stringByAppendingPathComponent:item.dropBoxFile.filename];
    return [NSString stringWithUTF8String:[localFilePath cStringUsingEncoding:NSUTF8StringEncoding]];
}

@end
