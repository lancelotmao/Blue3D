//
//  LocalViewController.h
//  Houyi
//
//  Created by Lance Mao on 4/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "FileBrowseViewController.h"

@interface LocalViewController : FileBrowseViewController
{
    IBOutlet UILabel* infoTitle;
    IBOutlet UIToolbar* mToolbar;
    IBOutlet UIBarButtonItem* mBarItemSelect;
    IBOutlet UIBarButtonItem* mBarItemTrash;
    
    UIBarButtonItem* mPlaceHolder;
    bool mIsSelectionMode;
    NSMutableArray* mSelectedItem;
}
@end
