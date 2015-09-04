//
//  HouyiAppDelegate.m
//  Houyi
//
//  Created by Lance Mao on 13/2/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#import "HouyiAppDelegate.h"
#import "Blue3DViewController.h"
#import <DropboxSDK/DropboxSDK.h>

@implementation HouyiAppDelegate

@synthesize mDemoListViewController, mDBVC, mBlue3DVC;

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    NSString* urlStr = [url absoluteString];
    NSLog(@"Houyi opening file, path = %@", urlStr);
    
    if ([urlStr hasPrefix:@"db"])
    {
        NSLog(@"Opening Dropbox URL");
        if ([[DBSession sharedSession] handleOpenURL:url]) {
            if ([[DBSession sharedSession] isLinked]) {
                NSLog(@"App linked successfully!");
                if(mDBVC) {
                    [mDBVC listFolder];
                }
            }
            return YES;
        }
        // Add whatever other url handling code your app requires here
        return NO;
    }
    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    FileItem* item = [FileItem new];
    item.localPath = url.path;
    
    Blue3DViewController* vc = [self getViewerVC];
    vc.mMode = EVMSingle;
    
    UIStoryboard *mainStoryboard = [UIStoryboard storyboardWithName:@"Main"
                                                             bundle: nil];
    UITabBarController *root = (UITabBarController*)[mainStoryboard
                                                    instantiateViewControllerWithIdentifier: @"roottab"];
    [self.window setRootViewController:root];
    
    [self.window setBackgroundColor:[UIColor whiteColor]];
    [self.window makeKeyAndVisible];
    
    // need to put after window visible or we can dispatch
    [[DataManager getInstance].items removeAllObjects];
    [[DataManager getInstance].items addObject:item];
    [DataManager getInstance].focus = 0;
    [root presentViewController:vc animated:NO completion:nil];
    
    return YES;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    DBSession* dbSession = [[DBSession alloc]
      initWithAppKey:@"dbjmsvqza5beaig"
      appSecret:@"529dztjxul8w91h"
      root:kDBRootDropbox];
    [DBSession setSharedSession:dbSession];
    
    // test for dynamic bundle
    NSBundle* bundle = [NSBundle bundleWithIdentifier:@"com.lance.Houyi"];
    NSError* e = nil;
    BOOL p = [bundle preflightAndReturnError:&e];
    if (p) {
        [bundle load];
    }
    
    return YES;
}
							
- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

- (Blue3DViewController*)getViewerVC
{
    if (mBlue3DVC == nil) {
        NSString* xibName = @"HouyiView";
        mBlue3DVC = [[Blue3DViewController alloc] initWithNibName:xibName bundle:[NSBundle mainBundle]];
    }
    
    return mBlue3DVC;
}

- (void)purchased
{
    [mDBVC purchased];
    [mDemoListViewController purchased];
}

- (bool)isPro
{
    bool res = [[NSUserDefaults standardUserDefaults] boolForKey:@"ispro"];
    NSLog(@"checking isPro %d", res);
    return res;
}

@end
