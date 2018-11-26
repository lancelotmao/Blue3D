//
//  HouyiCameraViewController.h
//  Houyi
//
//  Created by Lance Mao on 14/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "HouyiViewController.h"

@interface HouyiCameraViewController : HouyiViewController
{
    IBOutlet UIView* mCameraView;
    AVCaptureSession *mSession;
    AVCaptureVideoPreviewLayer *mCameraLayer;
    
    AVCaptureDeviceInput *mCaptureInput;
    AVCaptureStillImageOutput *mStillImageOutput;
    UIImage* mCapturedImage;
    UIDeviceOrientation mRunningOrientation;
}

@property (strong, nonatomic) UIImage* mCapturedImage;

- (void)startCamera;
- (void)stopCamera;
- (void)toggleCamera;
- (BOOL)isCameraRunning;
- (void)capture;

- (void)onCaptured;

@end
