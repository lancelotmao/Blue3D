//
//  Pocket3DViewController.m
//  Houyi
//
//  Created by Lance Mao on 23/1/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "Blue3DViewController.h"
#import "BlueSetting.h"
//#import "HouyiAppDelegate.h"
//#import "DataBaseManager.h"
#import "UIImage+HouyiUIImage.h"
#include "BlueWorld.h"

@implementation Blue3DViewController
{
    NSMutableArray* dataArray;
    int mCurShadingModeSetting;
    int mScreenShotRequestFocus;
}

@synthesize mMode;

- (void)viewDidLoad
{
    // changing delegate will trigger the previous controller's dealloc
    [DataManager getInstance].focusDelegate = self;
    
    [super viewDidLoad];

    dataArray = [[NSMutableArray alloc] init];
    [dataArray addObject:@"Default"];
    [dataArray addObject:@"Per Pixel Lighting"];
    [dataArray addObject:@"Solid"];
    [dataArray addObject:@"WireFrame"];
//    [dataArray addObject:@"Sci-fi"];
    
    UIImage* img = [UIImage imageNamed:@"ic_camera"];
    UIBarButtonItem* camera = [[UIBarButtonItem alloc] initWithImage:img style:UIBarButtonItemStylePlain target:self action:@selector(cameraClicked:)];
    UIBarButtonItem* menu = [[UIBarButtonItem alloc] initWithTitle:@"Menu" style:UIBarButtonItemStylePlain target:self action:@selector(menuClicked:)];
    self.navigationItem.rightBarButtonItems = @[menu, camera];
    [self setContinouslyRender];
    
    [self applySetting];
}

- (void)setContinouslyRender
{
    continuouslyRender = [BlueSetting getInstance].mShowFPS;
    [mGLKView setNeedsDisplay];
}

- (void)viewWillAppear:(BOOL)animated {
    loadCanceled = NO;
    [super viewWillAppear:animated];
    mBtnPause.hidden = YES;
    [self hideAllSetting];
    [mGLKView setDrawableMultisample:GLKViewDrawableMultisampleNone];
    [self startGotoFullScreenTimer];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [mFullScreenTimer invalidate];
    [super viewWillDisappear:animated];
}

- (void)onCreate
{
    [super onCreate];
    
//    [HouyiAppDelegate setRoot:mRoot];
    
    int width = mGLKView.frame.size.width;
    int height = mGLKView.frame.size.height;
    
    int mode = Settings::getInstance()->getShadingMode();
    [self updateShadingMode:mode];
    
    DataManager* dataMan = [DataManager getInstance];
    FileItem* item = [dataMan getFocusItem];
    if ([item.localPath hasSuffix:@".tmx"]) {
        mWorld = HouyiNew GameWorld();
    } else {
        BlueWorld* world = HouyiNew BlueWorld();
        world->setViewMode(mMode);
        mWorld = world;
    }
    
    mWorld->create(mRoot);
    mRoot->setWorld(mWorld);
    mRoot->onWindowChanged(width, height);
    
    mAxis = mWorld->addAxis();
    
    // first we need to set asset manager delegate to data manager
    // by default it is set to view controll
    assetMan = [HouyiAssetManager getInstance :[DataManager getInstance]];
    [assetMan loadAsset:item.localPath focusIndex:[DataManager getInstance].focus];
    [NSTimer scheduledTimerWithTimeInterval:1
                                     target:self
                                   selector:@selector(loadingStart)
                                   userInfo:nil
                                    repeats:NO];
}

- (IBAction)menuClicked:(id) sender
{
    mViewShowHide.hidden = YES;
    mViewControl.hidden = YES;
    mViewShading.hidden = YES;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
        [self showPopOver:sender];
    } else {
        [self showActionSheet];
    }
    [mFullScreenTimer invalidate];
}

- (bool)menuAvailable
{
    return mViewShowHide.hidden && mViewControl.hidden && mViewShading.hidden;
}

- (void)showActionSheet
{
    NSString* op1 = NSLocalizedString(@"show_hide", nil);
    NSString* op2 = NSLocalizedString(@"control", nil);
    NSString* op3 = NSLocalizedString(@"shading", nil);
    NSString* op4 = nil;
    if ([self isPresenting]) {
        op4 = NSLocalizedString(@"stop_presentation", nil);
    } else {
        op4 = NSLocalizedString(@"presentation", nil);
    }
    NSString* op5 = NSLocalizedString(@"screenshot", nil);
    NSString* op6 = nil;
    if (mGLKView.drawableMultisample == GLKViewDrawableMultisampleNone) {
        op6 = NSLocalizedString(@"Enable Multisample", nil);
    } else {
        op6 = NSLocalizedString(@"Disable Multisample", nil);
    }
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil
                                                             delegate:self
                                                    cancelButtonTitle:@"Cancel"
                                               destructiveButtonTitle:nil
                                                    otherButtonTitles:op1, op2, op3, op4, op5, op6, nil];
    [actionSheet showInView:self.view];
}

- (void)showPopOver:(id) sender
{
    SettingPopOverViewController *popView=[[SettingPopOverViewController alloc]initWithNibName:@"SettingPopOverViewController" bundle:nil];
    
    if (mMenuPopup == nil) {
        NSMutableArray* ops = [NSMutableArray new];
        [ops addObject:NSLocalizedString(@"show_hide", nil)];
        [ops addObject:NSLocalizedString(@"control", nil)];
        [ops addObject:NSLocalizedString(@"shading", nil)];
        if ([self isPresenting]) {
            [ops addObject:NSLocalizedString(@"stop_presentation", nil)];
        } else {
            [ops addObject:NSLocalizedString(@"presentation", nil)];
        }
        [ops addObject:NSLocalizedString(@"screenshot", nil)];
        if (mGLKView.drawableMultisample == GLKViewDrawableMultisampleNone) {
            [ops addObject:NSLocalizedString(@"Enable Multisample", nil)];
        } else {
            [ops addObject:NSLocalizedString(@"Disable Multisample", nil)];
        }
        popView.mOptions = ops;
        mMenuPopup = [[UIPopoverController alloc] initWithContentViewController:popView];
        [mMenuPopup presentPopoverFromBarButtonItem:(UIBarButtonItem *)sender
                           permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];
        popView.delegate = self;
    } else {
        [mMenuPopup dismissPopoverAnimated:YES];
        mMenuPopup = nil;
    }
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    [self settingSelected:buttonIndex];
}

- (void)popOverClicked:(int)index
{
    [self settingSelected:index];
    [mMenuPopup dismissPopoverAnimated:YES];
    mMenuPopup = nil;
}

- (void)settingSelected:(int)index
{
    if (index == 0) {
        [self updateSettingUI];
        mViewShowHide.hidden = NO;
    } else if (index == 1) {
        [self updateSettingUI];
        mViewControl.hidden = NO;
    } else if (index == 2) {
        [self updateSettingUI];
        mViewShading.hidden = NO;
    } else if (index == 3) {
        [self updateSettingUI];
        if (mWorld && mWorld->getCurrentCamera()) {
            if ([self isPresenting]) {
                mWorld->getCurrentCamera()->stopPresent();
            } else {
                mWorld->getCurrentCamera()->present();
            }
        }
    } else if (index == 4) {
        [self takeScreenShotAndGotoSingle];
    } else if (index == 5) {
        if (mGLKView.drawableMultisample == GLKViewDrawableMultisampleNone) {
            [mGLKView setDrawableMultisample:GLKViewDrawableMultisample4X];
        } else {
            [mGLKView setDrawableMultisample:GLKViewDrawableMultisampleNone];
        }
    }
    [self requestRender];
}

- (IBAction)done:(id) sender {
    loadCanceled = YES;
    [assetMan cancelAll];
    
    if (mMenuPopup) {
        [mMenuPopup dismissPopoverAnimated:YES];
        mMenuPopup = nil;
    }
    
    [self stopCamera];
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)applySetting
{
    bool showFPS = [BlueSetting getInstance].mShowFPS;
    mViewStats.hidden = !showFPS;
    
    bool isOn = [BlueSetting getInstance].mShowGrid;
    if (mGrid) {
        mGrid->setVisibility(isOn ? Houyi::SceneNode::EVisible : Houyi::SceneNode::EInvisible);
    }
    
    isOn = [BlueSetting getInstance].mShowAxis;
    if (mAxis) {
        mAxis->setVisibility(isOn ? Houyi::SceneNode::EVisible : Houyi::SceneNode::EInvisible);
    }
    
    isOn = [BlueSetting getInstance].mShowAABB;
    if (isOn) {
        mWorld->showAABB();
    } else {
        mWorld->hideAABB();
    }
    
    Houyi::Camera* camera = mWorld->getCurrentCamera();
    if (camera) {
        int c = camera->getPeekConstraint();
        isOn = [BlueSetting getInstance].mRotateX;
        if (isOn)
            c &= ~1;
        else
            c |= 1;
        isOn = [BlueSetting getInstance].mRotateY;
        if (isOn)
            c &= ~2;
        else
            c |= 2;
        camera->setPeekConstraint(c);
    }
    
    NSInteger mode = mShadingTable.indexPathForSelectedRow.row;
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:mode inSection:0];
    [mShadingTable selectRowAtIndexPath:indexPath animated:NO scrollPosition:UITableViewScrollPositionNone];
    Settings::getInstance()->setShadingMode(mRoot, mode);
    [self updateShadingMode:mode];
}

- (void)updateSettingUI
{
    [mSwitchFPS setOn:[BlueSetting getInstance].mShowFPS];
    [mSwitchGrid setOn:[BlueSetting getInstance].mShowGrid];
    [mSwitchAxis setOn:[BlueSetting getInstance].mShowAxis];
    [mSwitchAABB setOn:[BlueSetting getInstance].mShowAABB];
    [mSwitchX setOn:[BlueSetting getInstance].mRotateX];
    [mSwitchY setOn:[BlueSetting getInstance].mRotateY];
}

- (IBAction)showFPSValueChange:(UISwitch*)sender
{
    BOOL state = [sender isOn];
    [BlueSetting getInstance].mShowFPS = state;
    mViewStats.hidden = !state;
    [self setContinouslyRender];
}

- (IBAction)showGridValueChange:(UISwitch*)sender
{
    BOOL state = [sender isOn];
    [BlueSetting getInstance].mShowGrid = state;
    if (mGrid) {
        mGrid->setVisibility(state ? Houyi::SceneNode::EVisible : Houyi::SceneNode::EInvisible);
    }
    [self requestRender];
}

- (IBAction)showAxisValueChange:(UISwitch*)sender
{
    BOOL state = [sender isOn];
    [BlueSetting getInstance].mShowAxis = state;
    if (mAxis) {
        mAxis->setVisibility(state ? Houyi::SceneNode::EVisible : Houyi::SceneNode::EInvisible);
    }
    [self requestRender];
}

- (IBAction)showAABBValueChange:(UISwitch*)sender
{
    BOOL state = [sender isOn];
    [BlueSetting getInstance].mShowAABB = state;
    if (state) {
        mWorld->showAABB();
    } else {
        mWorld->hideAABB();
    }
    [self requestRender];
}

- (IBAction)showFourViewValueChange:(UISwitch*)sender
{
    BOOL state = [sender isOn];
    [BlueSetting getInstance].mShowFourView = state;
    mRoot->getRenderer()->setSingleViewPort(!state);
    [self requestRender];
}

- (IBAction)rotateXValueChange:(UISwitch*)sender
{
    BOOL state = [sender isOn];
    [BlueSetting getInstance].mRotateX = state;
    Houyi::Camera* camera = mWorld->getCurrentCamera();
    int c = camera->getPeekConstraint();
    if (state)
        camera->setPeekConstraint(c & ~1);
    else
        camera->setPeekConstraint(c | 1);
}

- (IBAction)rotateYValueChange:(UISwitch*)sender
{
    BOOL state = [sender isOn];
    [BlueSetting getInstance].mRotateY = state;
    Houyi::Camera* camera = mWorld->getCurrentCamera();
    int c = camera->getPeekConstraint();
    if (state)
        camera->setPeekConstraint(c & ~2);
    else
        camera->setPeekConstraint(c | 2);
}

- (IBAction)playClicked:(id)sender
{
    [self startGotoFullScreenTimer];
    Scene* scene = mWorld->getFocusScene();
    if (scene) {
        bool paused = scene->isSkeletonPaused();
        paused ? scene->resumeSkeleton() : scene->pauseSkeleton();
        [self updatePlayIcon :scene];
    }
}

- (void)updatePlayIcon :(Scene*)scene
{
    bool paused = scene->isSkeletonPaused();
    NSString* name = paused ? @"play.png" : @"pause.png";
    [mBtnPause setBackgroundImage:[UIImage imageNamed:name] forState:UIControlStateNormal];
}

- (void)hideAllSetting
{
    mViewShowHide.hidden = YES;
    mViewControl.hidden = YES;
    mViewShading.hidden = YES;
    if (mMenuPopup) {
        [mMenuPopup dismissPopoverAnimated:YES];
        mMenuPopup = nil;
    }
}

- (bool)isPresenting
{
    return mWorld && mWorld->getCurrentCamera()
            && mWorld->getCurrentCamera()->getAnimController().isPresenting();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self hideAllSetting];
    [super touchesBegan:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesEnded:touches withEvent:event];
    [self startGotoFullScreenTimer];
}

- (void) updateShadingMode:(int)mode
{
    if (mode != mCurShadingModeSetting) {
        Settings::getInstance()->setShadingMode(mRoot, mode);
        mCurShadingModeSetting = mode;
    }
}

- (void)startCamera
{
    if (mWorld) {
        mWorld->getRootView()->setVisibility(View::EInvisible);
    }
    [super startCamera];
}

- (void)stopCamera
{
    if (mWorld) {
        mWorld->getRootView()->setVisibility(View::EVisible);
    }
    [super stopCamera];
}

// tell the picker how many rows are available for a given component
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    return [dataArray count];
}

// tell the picker how many components it will have
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

// tell the picker the title for a given component
- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
    return [dataArray objectAtIndex: row];
}

#pragma mark table
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [dataArray count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    // Configure the cell...
    cell.backgroundColor = [UIColor clearColor];
    cell.textLabel.text = [dataArray objectAtIndex:indexPath.row];
    [cell.textLabel setTextColor:[UIColor whiteColor]];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self updateShadingMode:indexPath.row];
    [mGLKView setNeedsDisplay];
}

#pragma mark Screen shot
- (void)takeScreenShotAndGotoSingle
{
    [self capture];
    if (mWorld && ((BlueWorld*)mWorld)->getFilmstrip()) {
        ((BlueWorld*)mWorld)->getFilmstrip()->fadeOut(0, 0);
    }
    mScreenShotType = ESSSingle;
     mScreenshot = [mGLKView snapshot];
    [self shouldGotoSingle];
}

-(void)onScreenShotTaken:(UIImage*) image
{
    if (mScreenShotType == ESSThumbnail) {
        if (mScreenShotRequestFocus == [DataManager getInstance].focus) {
            [self onThumbnailTaken:image];
        }
    }
}

- (void)onThumbnailTaken:(UIImage*) image
{
//    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        // make it square
        ImagePtr houyiImage = ImageFactory::createImage(image);
        ImagePtr square = ImageFactory::createImage(256, 256);
        CanvasPtr canvas = CanvasFactory::getCanvas(square);
        canvas->drawImageCropCenter(houyiImage);
        
        // scale down
        ImagePtr target = ImageFactory::createImage(160, 128);
        CanvasPtr canvas2 = CanvasFactory::getCanvas(target);
        canvas2->drawImageCropCenter(square);
        
        // save image
        FileItem* item = [[DataManager getInstance] getFocusItem];
        item.thumbnail = target->getUIImage();
        NSString* thumbnailPath = [FileItem getThumbnailPath:item.localPath];
        NSData *imageData = UIImageJPEGRepresentation(target->getUIImage(), 1.0);
        if([imageData writeToFile:thumbnailPath atomically:YES]) {
        } else {
            LOGE("Cannot save image filePath = %s\n", [thumbnailPath cStringUsingEncoding:NSUTF8StringEncoding]);
        }
        
        // insert to database
//        [[DataBaseManager getSharedInstance] updateThumbnail:item:YES];
    
        if (mGrid)
        {
            bool isOn = [BlueSetting getInstance].mShowGrid;
            mGrid->setVisibility(isOn ? Houyi::SceneNode::EVisible : Houyi::SceneNode::EInvisible);
        }
        
        bool isOn = [BlueSetting getInstance].mShowAABB;
        if (isOn) {
            mWorld->showAABB();
        }

        ((BlueWorld*)mWorld)->getAdapter()->notifyDataChange();
        [self requestRender];
//    });
}

- (void)shouldGotoSingle
{
    if (([self isCameraRunning] && self.mCapturedImage == nil) || mScreenshot == nil) {
        return;
    }
    
    glkReady = NO;
    dispatch_async( dispatch_get_main_queue(), ^{
        if (mSIVC == nil) {
            NSString* xibName = @"SingleImageViewController";
            mSIVC = [[SingleImageViewController alloc] initWithNibName:xibName bundle:[NSBundle mainBundle]];
        }
        
        float screenScale = [[UIScreen mainScreen] scale];
        float w = mGLKView.frame.size.width;
        float h = mGLKView.frame.size.height;
        ImagePtr combined = ImageFactory::createImage(w*screenScale, h*screenScale);
        CanvasPtr canvas = CanvasFactory::getCanvas(combined);
        canvas->drawImageCropCenter(ImageFactory::createImage(self.mCapturedImage));
        canvas->drawImageCropCenter(ImageFactory::createImage(mScreenshot));
        
        mSIVC.mImage = combined->getUIImage();
        [self presentViewController:mSIVC animated:YES completion:nil];
    });
}

#pragma mark Camera
- (IBAction)cameraClicked:(id)sender
{
    [self toggleCamera];
}

- (void)onCaptured
{
    [self shouldGotoSingle];
}

#pragma mark Filmstrip
- (void)onFocusChanged :(int)newFocus :(int)oldFocus
{
    [NSTimer scheduledTimerWithTimeInterval:1
                                     target:self
                                   selector:@selector(loadingStart)
                                   userInfo:nil
                                    repeats:NO];
}

- (void)loadingStart
{
    if ([assetMan isLoading]) {
        DataManager* dataMan = [DataManager getInstance];
        FileItem* item = [dataMan.items objectAtIndex:dataMan.focus];
        string filePath = [item.localPath cStringUsingEncoding:NSUTF8StringEncoding];
        Scene* scene = SceneManager::getInstance()->findSceneByName(filePath);
        if (!scene) {
            // start loading indicator only when focus model is loading
            mProgress = 0;
            mLabelProgress.hidden = NO;
            mLabelProgress.text = nil;
            [self updateLoadingProgress];
            mProgressTimer = [NSTimer scheduledTimerWithTimeInterval:0.5
                                                             target:self
                                                           selector:@selector(updateLoadingProgress)
                                                           userInfo:nil
                                                            repeats:YES];
        }
    }
}

- (void)updateLoadingProgress
{
    mProgress = [[DataManager getInstance] getFocusProgress];
    [super updateLoadingProgress];
}

- (void)onFocusModelLoaded :(int)newFocus :(Scene*)scene
{
    if (!mWorld || !scene) {
        return;
    }
    
    dispatch_async( dispatch_get_main_queue(), ^{
        [mProgressTimer invalidate];
        mScene = scene;
        mLabelProgress.hidden = YES;
        mBtnPause.hidden = scene->getSkeletonCount() == 0;
        mWorld->clearScene();
        mWorld->insertScene(scene, 0);
        mWorld->setFocusScene(scene);
        
        mGrid = scene->addGrid();
        
        if ([BlueSetting getInstance].mShowAABB) {
            mWorld->showAABB();
        } else {
            mWorld->hideAABB();
        }
        
        if (scene->getSkeletonCount() > 0) {
            scene->resumeSkeleton();
            [self updatePlayIcon :scene];
        }

        [self applySetting];
        
        FileItem* item = [[DataManager getInstance] getFocusItem];
//        if (item.dropBoxFile)
//        {
//            if (mGrid)
//            {
//                mGrid->setVisibility(SceneNode::EInvisible);
//            }
//            mScreenShotRequestFocus = [DataManager getInstance].focus;
//            mScreenShotType = ESSThumbnail;
//            [self requestScreenShot:self];
//        }
    });
    [self requestRender];
}

#pragma mark Full Screen
- (IBAction)handleTapGesture:(UITapGestureRecognizer*)gestureRecognizer
{
    if (gestureRecognizer.numberOfTouches == 1) {
        mToolBar.alpha = 1;
        mBtnPause.alpha = 1;
        [self.navigationController setNavigationBarHidden:NO animated:NO];
        if (mWorld && ((BlueWorld*)mWorld)->getFilmstrip()) {
            ((BlueWorld*)mWorld)->getFilmstrip()->fadeIn(1, 0);
        }
        [self startGotoFullScreenTimer];
    }
    [self requestRender];
}

- (void)startGotoFullScreenTimer
{
    [mFullScreenTimer invalidate];
    mFullScreenTimer = [NSTimer scheduledTimerWithTimeInterval:3
                                     target:self
                                   selector:@selector(gotoFullScreen)
                                   userInfo:nil
                                    repeats:NO];
}

- (void)gotoFullScreen
{
    [UIView animateWithDuration:0.4
                     animations:^{
                         mToolBar.alpha = 0;
                         mBtnPause.alpha = 0;
                         [self.navigationController setNavigationBarHidden:YES animated:YES];
                     }
                     completion:^(BOOL finished){
                         // whatever
                     }];
    if (mWorld && ((BlueWorld*)mWorld)->getFilmstrip()) {
        ((BlueWorld*)mWorld)->getFilmstrip()->fadeOut(0, 0);
    }
    [self requestRender];
}

@end
