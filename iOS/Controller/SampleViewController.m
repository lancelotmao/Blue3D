//
//  SampleViewController.m
//  Blue3D
//
//  Created by lance  on 2018/11/23.
//  Copyright © 2018 lance. All rights reserved.
//

#import "SampleViewController.h"
#import "FileItem.h"
#import "DataManager.h"
#import "Blue3DViewController.h"

@interface SampleViewController () <UIDocumentPickerDelegate> {
    IBOutlet __weak UICollectionView* _collectionView;
    NSMutableArray* _fileItems;
}

@end

@implementation SampleViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    UINib *cellNIB = [UINib nibWithNibName:@"SampleCell" bundle:nil];
    [_collectionView registerNib:cellNIB forCellWithReuseIdentifier:@"SampleCell"];
    UIBarButtonItem* menu = [[UIBarButtonItem alloc] initWithTitle:@"Open" style:UIBarButtonItemStylePlain target:self action:@selector(menuClicked:)];
    self.navigationItem.rightBarButtonItem = menu;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.topItem.title = @"Blue 3D";
    
    DataManager* dataMan = [DataManager getInstance];
    [dataMan.items removeAllObjects];
    
    NSString *resourceDBFolderPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"samples"];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error;
    
    NSArray *fileList = [fileManager contentsOfDirectoryAtPath:resourceDBFolderPath error:&error];
    for (NSString *s in fileList) {
        NSString* p = [resourceDBFolderPath stringByAppendingPathComponent:s];
        NSArray *array = [fileManager contentsOfDirectoryAtPath:p error:&error];
        FileItem* item = [FileItem new];
        item.fileName = s;
        for (NSString* m in array) {
            NSString* path = [p stringByAppendingPathComponent:m];
            if ([path hasSuffix:@"obj"] || [path hasSuffix:@"dae"]
                || [path hasSuffix:@"3ds"] || [path hasSuffix:@"houyi"] || [path hasSuffix:@"stl"]) {
                item.localPath = path;
                item.thumbnail = [UIImage imageWithContentsOfFile:[p stringByAppendingPathComponent:@"thumbnail.png"]];
                break;
            }
        }
        [dataMan.items addObject:item];
    }
    [DataManager getInstance].itemCount = fileList.count;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return [DataManager getInstance].itemCount;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    UICollectionViewCell *cell = [collectionView
                                  dequeueReusableCellWithReuseIdentifier:@"SampleCell"
                                  forIndexPath:indexPath];
    NSInteger row = [indexPath row];
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

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    NSInteger row = [indexPath row];
    DataManager* dataMan = [DataManager getInstance];
    dataMan.focus = row;
    NSString* xibName = @"HouyiView";
    Blue3DViewController* vc = [[Blue3DViewController alloc] initWithNibName:xibName bundle:[NSBundle mainBundle]];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)menuClicked:(id)sender {
    UIDocumentPickerViewController *documentPicker = [[UIDocumentPickerViewController alloc] initWithDocumentTypes:@[@"public.item"]
                                                                                                            inMode:UIDocumentPickerModeImport];
    documentPicker.delegate = self;
    documentPicker.modalPresentationStyle = UIModalPresentationFormSheet;
    [self presentViewController:documentPicker animated:YES completion:nil];
}

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentAtURL:(NSURL *)url {
    if (controller.documentPickerMode == UIDocumentPickerModeImport) {
        FileItem* item = [FileItem new];
        item.localPath = url.path;
        
        // need to put after window visible or we can dispatch
        [[DataManager getInstance].items removeAllObjects];
        [[DataManager getInstance].items addObject:item];
        [DataManager getInstance].focus = 0;
        
        NSString* xibName = @"HouyiView";
        Blue3DViewController* vc = [[Blue3DViewController alloc] initWithNibName:xibName bundle:[NSBundle mainBundle]];
        vc.mMode = EVMSingle;
        [self.navigationController pushViewController:vc animated:YES];
    }
}
@end
