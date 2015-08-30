//
//  Pocket3DViewController.h
//  Houyi
//
//  Created by Lance Mao on 23/1/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiCameraViewController.h"
#import "SettingPopOverViewController.h"
#import "SingleImageViewController.h"
#import "FileItem.h"
#import "DataManager.h"
#import "iAd/ADBannerView.h"

@interface Blue3DViewController : HouyiCameraViewController<UIActionSheetDelegate,SettingPopOverDelegate, FocusDelegate>
{
    enum
    {
        ESSThumbnail = 0,
        ESSSingle
    };
    
    // View Mode
    enum
    {
        EVMNormal = 0,
        EVMSingle
    };
    
    IBOutlet UIToolbar* mToolBar;
    IBOutlet UIBarItem* mMenuItem;
    UIPopoverController* mMenuPopup;
    IBOutlet UIButton* mBtnPause;
    
    // show hide
    IBOutlet UIView* mViewStats;
    IBOutlet UIView* mViewShowHide;
    IBOutlet UISwitch* mSwitchFPS;
    IBOutlet UISwitch* mSwitchGrid;
    IBOutlet UISwitch* mSwitchAxis;
    IBOutlet UISwitch* mSwitchAABB;
    IBOutlet UISwitch* mSwitchFourView;
    
    // control
    IBOutlet UIView* mViewControl;
    IBOutlet UISwitch* mSwitchX;
    IBOutlet UISwitch* mSwitchY;
    
    // shading
    IBOutlet UIView* mViewShading;
    IBOutlet UIPickerView* mPickerShadingMode;
    IBOutlet UITableView* mShadingTable;
    
    // single view
    int mScreenShotType;
    SingleImageViewController* mSIVC;
    
    Scene* mScene;
    Houyi::SceneNode* mGrid;
    Houyi::SceneNode* mAxis;
    NSTimer* mFullScreenTimer;
}

@property (readwrite, nonatomic) int mMode;

- (IBAction)showFourViewValueChange:(UISwitch*)sender;

- (void)onFocusChanged :(int)newFocus :(int)oldFocus;
- (void)onFocusModelLoaded :(int)newFocus :(Scene*)scene;

@end
