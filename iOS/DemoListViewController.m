//
//  DemoListViewController.m
//  Houyi
//
//  Created by Lance Mao on 3/3/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#import "DemoListViewController.h"
#import "HouyiAppDelegate.h"
#import "DataManager.h"

@implementation DemoListViewController

static NSArray* NAME;
static NSArray* IMAGE_NAME;
static NSArray* DAE_NAME;

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
	// Do any additional setup after loading the view.
    
	mDelegate.mDemoListViewController = self;
    
    UINib *cellNIB = [UINib nibWithNibName:@"SampleCell" bundle:nil];
    [mCollectionView registerNib:cellNIB forCellWithReuseIdentifier:@"SampleCell"];
    
    DataManager* dataMan = [DataManager getInstance];
    [dataMan.items removeAllObjects];
    
    NSString *resourceDBFolderPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"samples"];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error;
    
    NSArray *fileList = [fileManager contentsOfDirectoryAtPath:resourceDBFolderPath error:&error];
    for (NSString *s in fileList) {
        NSString* p = [resourceDBFolderPath stringByAppendingPathComponent:s];
        NSArray *array = [fileManager contentsOfDirectoryAtPath:p error:&error];
        for (NSString* m in array) {
            NSString* path = [p stringByAppendingPathComponent:m];
            if ([path hasSuffix:@"obj"] || [path hasSuffix:@"dae"]
                || [path hasSuffix:@"3ds"] || [path hasSuffix:@"houyi"] || [path hasSuffix:@"stl"]) {
                FileItem* item = [FileItem new];
                item.localPath = path;
                item.fileName = s;
                item.thumbnail = [UIImage imageWithContentsOfFile:[p stringByAppendingPathComponent:@"thumbnail.png"]];
                [dataMan.items addObject:item];
            }
        }
        
    }
    [DataManager getInstance].itemCount = fileList.count;
}

-(void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.tabBarController.title = NSLocalizedString(@"Blue 3D", nil);
    self.title = NSLocalizedString(@"Samples", nil);
    navBar.topItem.title = self.tabBarController.title;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return [DataManager getInstance].itemCount;
}

-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView
                 cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewCell *cell = [collectionView
                                  dequeueReusableCellWithReuseIdentifier:@"SampleCell"
                                  forIndexPath:indexPath];
    int row = [indexPath row];
    FileItem* item = [DataManager getInstance].items[row];
    NSString* name = item.fileName;
    
    UIImageView* iv = (UIImageView*)[cell viewWithTag:100];
    iv.image = item.thumbnail;
    
    UILabel* label = (UILabel*)[cell viewWithTag:101];
    label.text = name;
    
    UIImageView* ivPlay = (UIImageView*)[cell viewWithTag:102];
//    if (row < 2) {
        ivPlay.hidden = YES;
//    }
    
    return cell;
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    int row = [indexPath row];
    DataManager* dataMan = [DataManager getInstance];
    dataMan.focus = row;
    HouyiAppDelegate* delegate = ((HouyiAppDelegate*)[UIApplication sharedApplication].delegate);
//    FileItem* item = [DataManager getInstance].items[row];
//    item.localPath = DAE_NAME[row];
//    item.isSample = YES;
	Blue3DViewController* vc = [delegate getViewerVC];
    vc.title = NAME[row];
    [self.tabBarController presentViewController:vc animated:YES completion:nil];
}

- (IBAction)about:(id) sender
{
    UIAlertView* _alert = [[UIAlertView alloc] init];
    [_alert setTitle:NSLocalizedString(@"about", nil)];
    [_alert setMessage:NSLocalizedString(@"about_content", nil)];
    [_alert setDelegate:self];
    [_alert addButtonWithTitle:NSLocalizedString(@"ok", nil)];
//    [_alert addButtonWithTitle:NSLocalizedString(@"check_it_out", nil)];
//    [_alert addButtonWithTitle:NSLocalizedString(@"later", nil)];
    [_alert show];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
//    if (buttonIndex == 0) {
//        NSURL *url = [NSURL URLWithString:@"http://code.google.com/p/openmobile3d/"];
//        [[UIApplication sharedApplication] openURL:url];
//    } else if (buttonIndex == 1) {
//        // No do nothing
//    }
}

@end
