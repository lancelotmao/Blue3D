//
//  HouyiViewController.mm
//  Houyi
//
//  Created by Lance Mao on 4/3/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#import <UIKit/UIImage.h>
#import "HouyiViewController.h"
#import "HouyiAssetManager.h"

#ifdef __cplusplus
#include <Houyi/Houyi.h>
using namespace Houyi;
#endif

@implementation HouyiViewController
{
    NSString* mFilePath;
    float mLastPinch;
    CGPoint mLastPan;
}

@synthesize mIsRendering;

- (void)viewDidLoad
{
    [super viewDidLoad];
    mSettings = Settings::getInstance();
    assetMan = [HouyiAssetManager getInstance:self];
    UIBarButtonItem* save = [[UIBarButtonItem alloc] initWithTitle:@"Save" style:UIBarButtonItemStylePlain target:self action:@selector(takeScreenShot)];
    self.navigationItem.rightBarButtonItem = save;
    mLastPinch = 1;
    mPinchDetector.enabled = Settings::getInstance()->usePlatformGesture();
    mPanDetector.enabled = mPinchDetector.enabled;
    glkReady = NO;
    mRenderLock = [NSLock new];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
    
    // Dispose of any resources that can be recreated.
}

- (void)onCreate
{
    mRoot = HouyiNew Root();
    mRoot->init();
    mRoot->onWindowCreated();
    
    float contentScale = 1.0f;
    if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)]) {
        contentScale = [[UIScreen mainScreen] scale];
    }
    mRoot->getRenderer()->setScreenBufferScale(contentScale);
    
    int width = mGLKView.frame.size.width;
    int height = mGLKView.frame.size.height;
    mRoot->onWindowChanged(width, height);
    
    [mGLKView setNeedsDisplay];
}

- (void)onDestroy
{
    [EAGLContext setCurrentContext:self.context];
    
    mRoot->mAutoFinalize = true;
    HouyiDelete(mRoot);
    mRoot = 0;
    mWorld = 0;
    dump_allocator_list();
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)requestRender
{
    dispatch_async( dispatch_get_main_queue(), ^{
        [mGLKView setNeedsDisplay];
    });
}

- (void)viewDidLayoutSubviews
{
    if (!mRoot)
    {
        self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
        if (!self.context) {
            NSLog(@"Failed to create ES context");
        }
        
        mGLKView.context = self.context;
        mGLKView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        mGLKView.opaque = NO;

        [EAGLContext setCurrentContext:self.context];
        
        [self onCreate];
    }

    [super viewDidLayoutSubviews];
}

- (void)viewDidAppear:(BOOL)animated
{
    glkReady = YES;
    [mGLKView setNeedsDisplay];
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    glkReady = NO;
    [super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [self onDestroy];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    int width = mGLKView.frame.size.width;
    int height = mGLKView.frame.size.height;
    mRoot->onWindowChanged(width, height);
    [mGLKView setNeedsDisplay];
}

#pragma mark - GLKView and GLKViewController delegate methods
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if (mIsRendering)
    {
        return;
    }
    
    // lock rendering thread
    mIsRendering = true;
    
    if (!mRoot || !glkReady)
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        mIsRendering = false;
        return;
    }
    
    [mRenderLock lock];
    
    mRoot->render();
    
    Renderer* renderer = mRoot->getRenderer();
    
    int pc = renderer->getPointCount();
    int lc = 0;
    int vc = 0;
    [mLabelFPS setText:[NSString stringWithFormat:@"FPS: %d", mRoot->getFPS()]];
    if (mWorld && mWorld->getFocusScene()) {
        SceneStats& stat = mWorld->getFocusScene()->getStats();
        [mLabelVCount setText:[NSString stringWithFormat:@"Vertex Count: %d", stat.mVertexCount]];
        lc = stat.mLineCount;
        vc = stat.mTriangleCount;
    }

    NSMutableString* ms = [[NSMutableString alloc] init];
    if (pc > 0)
    {
        [ms appendString:[NSString stringWithFormat:@"Point: %d", pc]];
    }
    if (lc > 0)
    {
        if ([ms length] > 0)
            [ms appendString:@", "];
        [ms appendString:[NSString stringWithFormat:@"Line: %d", lc]];
    }
    if (vc > 0)
    {
        if ([ms length] > 0)
            [ms appendString:@", "];
        [ms appendString:[NSString stringWithFormat:@"Triangle: %d", vc]];
    }
    [mLabelElement setText:ms];
    
    if (mScreenShotDelegate && !mWorld->getFocusScene()->getTextureManager()->hasPendingTexture())
    {
        mWorld->getRootView()->setVisibility(SceneNode::EInvisible);
        mRoot->render();
        mScreenshot = [self getScreenShot];
        [mScreenShotDelegate onScreenShotTaken:mScreenshot];
        mScreenShotDelegate = nil;
        mWorld->getRootView()->setVisibility(SceneNode::EVisible);
        mRoot->render();
    }
    
    if (continuouslyRender || mRoot->isRenderRequested())
    {
        dispatch_async( dispatch_get_main_queue(), ^{
            if (mRoot)
            {
                [mGLKView setNeedsDisplay];
                mRoot->requestRender(false);
            }
        });
    }
    
    mIsRendering = false;
    
    [mRenderLock unlock];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (mRoot)
    {
        MotionEvent e(0, 0, MotionEvent::TYPE_DOWN);
        int allCount = [[event allTouches] count];
        if (allCount == 1)
        {
            CGPoint pt = [[touches anyObject] locationInView:mGLKView];
            e.setX(pt.x);
            e.setY(pt.y);
        }
        else
        {
            NSEnumerator *enumerator = [[event allTouches] objectEnumerator];
            id object;
            int i = 0;
            
            while ((object = [enumerator nextObject]))
            {
                UITouch* touch = (UITouch*)object;
                float x = [touch locationInView:mGLKView].x;
                float y = [touch locationInView:mGLKView].y;
                e.setX(x, i);
                e.setY(y, i);
                i++;
            }
            e.mTouchCount = allCount;
        }
        mRoot->onTouch(e);
        [mGLKView setNeedsDisplay];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (mRoot)
    {
        CGPoint pt = [[touches anyObject] locationInView:mGLKView];
        MotionEvent e(pt.x, pt.y, MotionEvent::TYPE_MOVE);
        if (!Settings::getInstance()->usePlatformGesture() && [touches count] == 2)
        {
            UITouch* touch = [[touches allObjects] objectAtIndex:1];
            float x = [touch locationInView:mGLKView].x;
            float y = [touch locationInView:mGLKView].y;
            e.setX(x, 1);
            e.setY(y, 1);
            e.mTouchCount = 2;
        }
        mRoot->onTouch(e);
        [mGLKView setNeedsDisplay];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (mRoot)
    {
        CGPoint pt = [[touches anyObject] locationInView:mGLKView];
        MotionEvent e(pt.x, pt.y, MotionEvent::TYPE_UP);
        e.mTouchCount = [[event touchesForView:mGLKView] count];
        mRoot->onTouch(e);
        [mGLKView setNeedsDisplay];
    }
}

- (IBAction)handlePinchGesture:(UIPinchGestureRecognizer*)gestureRecognizer 
{
    if (Settings::getInstance()->usePlatformGesture())
    {
        if([gestureRecognizer state] == UIGestureRecognizerStateBegan)
        {
            mLastPinch = [gestureRecognizer scale];
        }
        float newScale = [gestureRecognizer scale];
        float del = newScale / mLastPinch;
        mRoot->getWorld()->getGestureRecognizer().handlePinch(del);
        mLastPinch = newScale;
        [mGLKView setNeedsDisplay];
    }
}

- (IBAction)handlePanGesture:(UIPanGestureRecognizer*)gestureRecognizer
{
    if (Settings::getInstance()->usePlatformGesture())
    {
        CGPoint trans = [gestureRecognizer translationInView:mGLKView];
        if ([gestureRecognizer state] == UIGestureRecognizerStateBegan) {
            mLastPan = trans;
        }
        float w = mGLKView.frame.size.width;
        float h = mGLKView.frame.size.height;
        float dx = trans.x - mLastPan.x;
        float dy = trans.y - mLastPan.y;
        mRoot->getWorld()->getGestureRecognizer().handlePan(dx/w, dy/h);
        mLastPan = trans;
        [mGLKView setNeedsDisplay];
    }
}

-(void)requestScreenShot:(id<HouyiScreenShotDelegate>) delegate
{
    mScreenShotDelegate = delegate;
}

-(void) takeScreenShot
{
    [self requestScreenShot:self];
}

-(UIImage*) getScreenShot
{
    int s = 1;
    UIScreen* screen = [ UIScreen mainScreen ];
    if ( [ screen respondsToSelector:@selector(scale) ] )
        s = (int) [ screen scale ];
    
    const int w = mGLKView.frame.size.width;
    const int h = mGLKView.frame.size.height;
    const NSInteger myDataLength = w * h * 4 * s * s;
    // allocate array and read pixels into it.
    GLubyte *buffer = (GLubyte *) malloc(myDataLength);
    glReadPixels(0, 0, w*s, h*s, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    // gl renders "upside down" so swap top to bottom into new array.
    // there's gotta be a better way, but this works.
    GLubyte *buffer2 = (GLubyte *) malloc(myDataLength);
    for(int y = 0; y < h*s; y++)
    {
        memcpy( buffer2 + (h*s - 1 - y) * w * 4 * s, buffer + (y * 4 * w * s), w * 4 * s );
    }
    free(buffer); // work with the flipped buffer, so get rid of the original one.
    
    // make data provider with data.
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer2, myDataLength, NULL);
    // prep the ingredients
    int bitsPerComponent = 8;
    int bitsPerPixel = 32;
    int bytesPerRow = 4 * w * s;
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaLast;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    // make the cgimage
    CGImageRef imageRef = CGImageCreate(w*s, h*s, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpaceRef, bitmapInfo, provider, NULL, NO, renderingIntent);
    // then make the uiimage from that
    UIImage *myImage = [ UIImage imageWithCGImage:imageRef scale:s orientation:UIImageOrientationUp ];
    
    // clean up
    CGImageRelease( imageRef );
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpaceRef);
    // IMPORTANT: crash on 7.1 if we call free(buffer2);
    return myImage;
}

- (void)imageSaved:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {
    NSString *message;
    NSString *title;
    if (!error) {
        title = NSLocalizedString(@"Information", @"");
        message = NSLocalizedString(@"Image successfully saved to Photo Album", @"");
    } else {
        title = NSLocalizedString(@"Failed", @"");
        message = [error description];
    }
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title
                                                    message:message
                                                   delegate:nil
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"")
                                          otherButtonTitles:nil];
    [alert show];
}

-(void)onScreenShotTaken:(UIImage*) image
{
    UIImageWriteToSavedPhotosAlbum(image, self, @selector(imageSaved: didFinishSavingWithError: contextInfo:),nil);
}

-(void) loadDataSync:(NSString*) filePath : (World*) world
{
    NSData* data = [HouyiUtils getDataFromFile:filePath];
    string strFilePath = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
    Loader* loader = Loader::getLoader(strFilePath);
    Scene* scene = loader->load((const char*)[data bytes], [data length]);
    world->addScene(scene);
}

-(void) loadData:(NSString*) filePath :(int)focus;
{
    mFilePath = filePath;
    [assetMan loadAsset:mFilePath focusIndex:focus];
    [NSTimer scheduledTimerWithTimeInterval:1
                                                     target:self
                                                   selector:@selector(loadingStart)
                                                   userInfo:nil
                                                    repeats:NO];
}

- (Scene*)loadDataSync:(NSString*) filePath
{
    return [assetMan loadAssetSync:filePath];
}

- (void)loadingStart
{
    if ([assetMan isLoading]) {
        // start loading indicator only when there is still loading task pending
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

- (void)updateLoadingProgress
{
    int p = (int)(mProgress * 100);
    if (p > 95)
        p = 100;
    mLabelProgress.text = [NSString stringWithFormat:@"%d %@", p, @"%"];
    if (p >= 100) {
        dispatch_async( dispatch_get_main_queue(), ^{
            [mProgressTimer invalidate];
            mLabelProgress.hidden = YES;
        });
    }
}

- (void)onLoadProgress:(LoadOperation*)op :(float)progress
{
    mProgress = progress;
}

- (void)onLoadFinished:(LoadOperation*)op :(Scene*)scene;
{
    mProgress = 100;
    dispatch_async( dispatch_get_main_queue(), ^{
        [mProgressTimer invalidate];
        mLabelProgress.hidden = YES;

        if (mWorld) {
            mWorld->addScene(scene);
            mWorld->setFocusScene(scene);
            SceneManager::getInstance()->addScene(scene);
        }
    });
    [self requestRender];
}

@end
