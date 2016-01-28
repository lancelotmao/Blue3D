//
//  HouyiAppDelegate.h
//  Houyi
//
//  Created by Lance Mao on 13/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DemoListViewController.h"
#import "DropBoxViewController.h"
#import "Blue3DViewController.h"

@interface HouyiAppDelegate : UIResponder <UIApplicationDelegate>
{
    Blue3DViewController* mBlue3DVC;
    DemoListViewController* mDemoListViewController;
    DropBoxViewController* mDBVC;    
}

@property (strong, nonatomic) UIWindow *window;
@property (retain, nonatomic) Blue3DViewController* mBlue3DVC;
@property (retain, nonatomic) DemoListViewController* mDemoListViewController;
@property (retain, nonatomic) DropBoxViewController* mDBVC;

- (Blue3DViewController*)getViewerVC;
- (void)purchased;
- (bool)isPro;
+ (Root*)getRoot;
+ (void)setRoot:(Root*)root;

@end
