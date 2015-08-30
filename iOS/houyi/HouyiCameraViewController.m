//
//  HouyiCameraViewController.m
//  Houyi
//
//  Created by Lance Mao on 14/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiCameraViewController.h"
#import <ImageIO/CGImageProperties.h>
#import "UIImage+HouyiUIImage.h"

@implementation HouyiCameraViewController

@synthesize mCapturedImage;

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    mStillImageOutput = [AVCaptureStillImageOutput new];
    NSDictionary *outputSettings = [[NSDictionary alloc] initWithObjectsAndKeys:AVVideoCodecJPEG,AVVideoCodecKey,nil];
    [mStillImageOutput setOutputSettings:outputSettings];
    
    [self initCamera];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    if ([self isCameraRunning]) {
        [self stopCamera];
        [self startCamera];
    }
}

- (void)initCamera
{
    if (mSession) {
        [mSession removeOutput:mStillImageOutput];
        [mSession removeInput:mCaptureInput];
    }
    
    mCaptureInput = [AVCaptureDeviceInput deviceInputWithDevice:[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo] error:nil];
    if (mCaptureInput) {
        mSession = [[AVCaptureSession alloc] init];
        [mSession addInput:mCaptureInput];
        [mSession addOutput:mStillImageOutput];
    }
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [super didRotateFromInterfaceOrientation:fromInterfaceOrientation];
    if ([mSession isRunning]) {
        [mSession stopRunning];
        [mCameraLayer removeFromSuperlayer];
        [self startCamera];
    }
}

- (void)startCamera
{
    mRunningOrientation = [[UIDevice currentDevice] orientation];
    [self initCamera];
    [mSession startRunning];
    Settings::getInstance()->setClearColor(Color(0, 0, 0, 0));
    mCameraLayer = [AVCaptureVideoPreviewLayer layerWithSession: mSession];
    [self updateCameraLayer];
    [mCameraView.layer addSublayer:mCameraLayer];
}

- (void)stopCamera
{
    [mSession stopRunning];
    [mCameraLayer removeFromSuperlayer];
    Settings::getInstance()->setDefClearColor();
}

- (void)toggleCamera
{
    (mSession && mSession.isRunning) ? [self stopCamera] : [self startCamera];
    [mGLKView setNeedsDisplay];
}

- (BOOL)isCameraRunning
{
    return mSession && [mSession isRunning];
}

- (void)updateCameraLayer
{
    mCameraLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    mCameraLayer.frame = mCameraView.bounds;
    float x = mCameraView.frame.origin.x;
    float y = mCameraView.frame.origin.y;
    float w = mCameraView.frame.size.width;
    float h = mCameraView.frame.size.height;
    CATransform3D transform = CATransform3DIdentity;
    if (UIDeviceOrientationLandscapeLeft == [[UIDevice currentDevice] orientation]) {
        mCameraLayer.frame = CGRectMake(x, y, h, w);
        transform = CATransform3DTranslate(transform, (w - h) / 2, (h - w) / 2, 0);
        transform = CATransform3DRotate(transform, -M_PI/2, 0, 0, 1);
    } else if (UIDeviceOrientationLandscapeRight == [[UIDevice currentDevice] orientation]) {
        mCameraLayer.frame = CGRectMake(x, y, h, w);
        transform = CATransform3DTranslate(transform, (w - h) / 2, (h - w) / 2, 0);
        transform = CATransform3DRotate(transform, M_PI/2, 0, 0, 1);
    } else if (UIDeviceOrientationPortraitUpsideDown == [[UIDevice currentDevice] orientation]) {
        mCameraLayer.frame = mCameraView.bounds;
        transform = CATransform3DMakeRotation(M_PI, 0, 0, 1);
    } else {
        mCameraLayer.frame = mCameraView.bounds;
    }
    mCameraLayer.transform  = transform;
}

- (void)capture
{
    mCapturedImage = nil;
    
    AVCaptureConnection *videoConnection = nil;
    for (AVCaptureConnection *connection in mStillImageOutput.connections)
    {
        for (AVCaptureInputPort *port in [connection inputPorts])
        {
            if ([[port mediaType] isEqual:AVMediaTypeVideo] )
            {
                videoConnection = connection;
                break;
            }
        }
        if (videoConnection) { break; }
    }
    
    if (videoConnection == nil) {
        NSLog(@"Failed to capture. NULL videoConnection");
        return;
    }
    
    NSLog(@"about to request a capture from: %@", mStillImageOutput);
    [mStillImageOutput captureStillImageAsynchronouslyFromConnection:videoConnection
                                                  completionHandler: ^(CMSampleBufferRef imageSampleBuffer, NSError *error)
     {
         if (imageSampleBuffer == nil) {
             NSLog(@"Failed to capture. NULL imageSampleBuffer");
             return;
         }
         
         CFDictionaryRef exifAttachments = (CFDictionaryRef)CMGetAttachment( imageSampleBuffer, kCGImagePropertyExifDictionary, NULL);
         if (exifAttachments)
         {
             // Do something with the attachments.
             NSLog(@"attachements: %@", exifAttachments);
         }
         else
             NSLog(@"no attachments");
         
         NSData *imageData = [AVCaptureStillImageOutput jpegStillImageNSDataRepresentation:imageSampleBuffer];
         mCapturedImage = [[UIImage alloc] initWithData:imageData];

         if (UIDeviceOrientationPortrait == [[UIDevice currentDevice] orientation]) {
             mCapturedImage = [mCapturedImage rotateData:mCapturedImage:0];
         } else if (UIDeviceOrientationPortraitUpsideDown == [[UIDevice currentDevice] orientation]) {
             mCapturedImage = [mCapturedImage rotateData:mCapturedImage:1];
         } else if (UIDeviceOrientationLandscapeLeft == [[UIDevice currentDevice] orientation]) {
             mCapturedImage = [mCapturedImage removeOrientationOfImage];
         } else {
             mCapturedImage = [mCapturedImage removeOrientationOfImage];
             mCapturedImage = [mCapturedImage rotateData:mCapturedImage:2];
         }
         
         [self onCaptured];
     }];
}

- (void)onCaptured
{
}

@end
