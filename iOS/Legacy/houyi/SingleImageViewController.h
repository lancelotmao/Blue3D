//
//  SingleImageViewController.h
//  Houyi
//
//  Created by Lance Mao on 15/3/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SingleImageViewController : UIViewController
{
    IBOutlet UIImageView* mIV;
}

@property (strong, nonatomic) UIImage* mImage;

@end
