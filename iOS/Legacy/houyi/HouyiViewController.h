//
//  HouyiViewController.h
//  Houyi
//
//  Created by Lance Mao on 4/3/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#import "HouyiAssetManager.h"

#ifdef __cplusplus
#include <Houyi/Houyi.h>
using namespace Houyi;
#endif

@protocol HouyiScreenShotDelegate <NSObject>

@optional

-(void)onScreenShotTaken:(UIImage*) image;

@end

@interface HouyiViewController : UIViewController<HouyiScreenShotDelegate, LoadDelegate>
{
    IBOutlet GLKView* mGLKView;
    IBOutlet UILabel* mLabelProgress;
    IBOutlet UILabel* mLabelFPS;
    IBOutlet UILabel* mLabelVCount;
    IBOutlet UILabel* mLabelElement;
    IBOutlet UIPinchGestureRecognizer* mPinchDetector;
    IBOutlet UIPanGestureRecognizer* mPanDetector;
    
    id<HouyiScreenShotDelegate> mScreenShotDelegate;
    UIActivityIndicatorView* mLoadingIndicator;
    NSTimer* mProgressTimer;
    NSLock* mRenderLock;
    bool loadCanceled;
    bool continuouslyRender;
    UIImage* mScreenshot;
    // due to auto layout glk size is incorrect in viewWillAppear
    // we need to delay rendering until viewDidAppear
    bool glkReady;
    
    HouyiAssetManager* assetMan;
    float mProgress;
    
#ifdef __cplusplus
    Root* mRoot;
    Settings* mSettings;
    World* mWorld;
#endif
}

@property (strong, nonatomic) EAGLContext *context;
// ios 7.1 issue. sometimes GL is drawn in a separate thread (don't know why)
@property (readonly, atomic) bool mIsRendering;

// view life cycle
- (void)onCreate;
- (void)onDestroy;

// By default set render mode to 'WHEN DIRTY'
- (void)requestRender;

// iOS GL call back for rendering
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect;

// screen shot
- (void)requestScreenShot:(id<HouyiScreenShotDelegate>) delegate;
- (void)takeScreenShot;
- (UIImage*)getScreenShot;
- (void)onScreenShotTaken:(UIImage*) image;

// load in current thread
- (Scene*)loadDataSync:(NSString*) filePath;

// loading happens in background thread
// focus is optional, used in those app that models are indexed
- (void)loadData:(NSString*) filePath :(int)focus;

// give some time before we show progress label
// because if we have a simple model loading happens very fast
// there is no need to show progress (if shown, it will blink) at all
- (void)loadingStart;
- (void)updateLoadingProgress;

// operation call back
- (void)onLoadProgress:(LoadOperation*)op :(float)progress;
- (void)onLoadFinished:(LoadOperation*)op :(Scene*)scene;

@end
