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

@property (weak, nonatomic) IBOutlet UIView* mCameraView;
@property (strong, nonatomic) UIImage* mCapturedImage;
@property (strong, nonatomic) AVCaptureSession *session;
@property (strong, nonatomic) AVCaptureVideoPreviewLayer *mCameraLayer;
@property (strong, nonatomic) AVCaptureDeviceInput *mCaptureInput;
@property (strong, nonatomic) AVCaptureStillImageOutput *mStillImageOutput;

- (void)startCamera;
- (void)stopCamera;
- (void)toggleCamera;
- (BOOL)isCameraRunning;
- (void)capture;

- (void)onCaptured;

@end
