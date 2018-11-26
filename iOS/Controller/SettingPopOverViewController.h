//
//  SettingPopOverViewController.h
//  Houyi
//
//  Created by Lance Mao on 22/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol SettingPopOverDelegate <NSObject>
@required
-(void)popOverClicked:(int)index;
@end

@interface SettingPopOverViewController : UITableViewController
{
    NSMutableArray* mOptions;
}

@property (nonatomic, weak) id<SettingPopOverDelegate> delegate;
@property (nonatomic, strong) NSMutableArray* mOptions;

@end
