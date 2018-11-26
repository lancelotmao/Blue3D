//
//  iCloudViewController.m
//  Blue3D
//
//  Created by lance  on 2018/11/23.
//  Copyright © 2018 lance. All rights reserved.
//

#import "iCloudViewController.h"

@interface iCloudViewController ()

@end

@implementation iCloudViewController

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.topItem.title = @"iCloud";
}

@end
