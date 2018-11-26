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
    self.mStillImageOutput = [AVCaptureStillImageOutput new];
    NSDictionary *outputSettings = [[NSDictionary alloc] initWithObjectsAndKeys:AVVideoCodecJPEG,AVVideoCodecKey,nil];
    [self.mStillImageOutput setOutputSettings:outputSettings];
    [self initCamera];
}

- (void)initCamera
{
    if (!_session) {
        self.mCaptureInput = [AVCaptureDeviceInput deviceInputWithDevice:[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo] error:nil];
        if (self.mCaptureInput) {
            _session = [[AVCaptureSession alloc] init];
            [_session addInput:self.mCaptureInput];
            [_session addOutput:self.mStillImageOutput];
        }
    }
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator {
    BOOL wasRunning = [self isCameraRunning];
    [self stopCamera];
    [coordinator animateAlongsideTransition:^(id  _Nonnull context) {
        
    } completion:^(id  _Nonnull context) {
        if (wasRunning) {
            [self startCamera];
        }
    }];
    
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
}

- (void)viewWillLayoutSubviews {
    [self updateCameraLayer];
}

- (void)startCamera
{
    [self initCamera];
    [_session startRunning];
    Settings::getInstance()->setClearColor(Color(0, 0, 0, 0));
    if (!self.mCameraLayer)
        self.mCameraLayer = [AVCaptureVideoPreviewLayer layerWithSession: _session];
    [self updateCameraLayer];
    [self.mCameraView.layer addSublayer:self.mCameraLayer];
}

- (void)stopCamera
{
    [_session stopRunning];
    [self.mCameraLayer removeFromSuperlayer];
    Settings::getInstance()->setDefClearColor();
}

- (void)toggleCamera
{
    (_session && _session.isRunning) ? [self stopCamera] : [self startCamera];
    [mGLKView setNeedsDisplay];
}

- (BOOL)isCameraRunning
{
    return _session && [_session isRunning];
}

- (void)updateCameraLayer
{
    self.mCameraLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    self.mCameraLayer.frame = self.mCameraView.bounds;
//    float x = self.mCameraView.frame.origin.x;
//    float y = self.mCameraView.frame.origin.y;
    float w = self.mCameraView.frame.size.width;
    float h = self.mCameraView.frame.size.height;
    CATransform3D transform = CATransform3DIdentity;
    if (UIDeviceOrientationLandscapeLeft == [[UIDevice currentDevice] orientation]) {
//        self.mCameraLayer.frame = CGRectMake(x, y, h, w);
        transform = CATransform3DTranslate(transform, (w - h) / 2, (h - w) / 2, 0);
        transform = CATransform3DRotate(transform, -M_PI/2, 0, 0, 1);
    } else if (UIDeviceOrientationLandscapeRight == [[UIDevice currentDevice] orientation]) {
//        self.mCameraLayer.frame = CGRectMake(x, y, h, w);
        transform = CATransform3DTranslate(transform, (w - h) / 2, (h - w) / 2, 0);
        transform = CATransform3DRotate(transform, M_PI/2, 0, 0, 1);
    } else if (UIDeviceOrientationPortraitUpsideDown == [[UIDevice currentDevice] orientation]) {
        self.mCameraLayer.frame = self.mCameraView.bounds;
        transform = CATransform3DMakeRotation(M_PI, 0, 0, 1);
    } else {
        self.mCameraLayer.frame = self.mCameraView.bounds;
    }
    self.mCameraLayer.transform  = transform;
}

- (void)capture
{
    if (![self isCameraRunning]) {
        return;
    }
    
    mCapturedImage = nil;
    
    AVCaptureConnection *videoConnection = nil;
    for (AVCaptureConnection *connection in self.mStillImageOutput.connections)
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
    
    HouyiCameraViewController* __weak weakSelf = self;
    NSLog(@"about to request a capture from: %@", self.mStillImageOutput);
    [self.mStillImageOutput captureStillImageAsynchronouslyFromConnection:videoConnection
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
         weakSelf.mCapturedImage = [[UIImage alloc] initWithData:imageData];

         if (UIDeviceOrientationPortrait == [[UIDevice currentDevice] orientation]) {
             weakSelf.mCapturedImage = [weakSelf.mCapturedImage rotateData:weakSelf.mCapturedImage:0];
         } else if (UIDeviceOrientationPortraitUpsideDown == [[UIDevice currentDevice] orientation]) {
             weakSelf.mCapturedImage = [weakSelf.mCapturedImage rotateData:weakSelf.mCapturedImage:1];
         } else if (UIDeviceOrientationLandscapeLeft == [[UIDevice currentDevice] orientation]) {
             weakSelf.mCapturedImage = [weakSelf.mCapturedImage removeOrientationOfImage];
         } else {
             weakSelf.mCapturedImage = [weakSelf.mCapturedImage removeOrientationOfImage];
             weakSelf.mCapturedImage = [weakSelf.mCapturedImage rotateData:weakSelf.mCapturedImage:2];
         }
         
         [self onCaptured];
     }];
}

- (void)onCaptured
{
}

@end
