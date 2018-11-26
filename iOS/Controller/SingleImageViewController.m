//
//  SingleImageViewController.m
//  Houyi
//
//  Created by Lance Mao on 15/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "SingleImageViewController.h"

@interface SingleImageViewController ()

@end

@implementation SingleImageViewController

@synthesize mImage;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [mIV setImage:mImage];
}

- (IBAction)done:(id) sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)share:(id) sender {
    NSArray *activityItems = nil;
    activityItems = @[@"", mImage];
    
    UIActivityViewController *activityController = [[UIActivityViewController alloc] initWithActivityItems:activityItems applicationActivities:nil];
    [self presentViewController:activityController animated:YES completion:nil];
}

@end
