//
//  HouyiIAPManager.m
//  Houyi
//
//  Created by Lance Mao on 22/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import "HouyiIAPManager.h"
#import "HouyiAppDelegate.h"

#define kInAppPurchaseProUpgradeProductId @"com.lance.blue3d.pro"
@implementation HouyiIAPManager

static HouyiIAPManager* mIns = nil;

+(HouyiIAPManager*)getInstance
{
    if (mIns == nil) {
        mIns = [HouyiIAPManager new];
    }
    return mIns;
}

+(int)getMaxModelCountForFree
{
    return 10;
}

- (void)requestProUpgradeProductData
{
    NSSet *productIdentifiers = [NSSet setWithObject:kInAppPurchaseProUpgradeProductId];
    productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    productsRequest.delegate = self;
    [productsRequest start];
    
    // we will release the request object in the delegate callback
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray *products = response.products;
    proUpgradeProduct = [products count] == 1 ? [products objectAtIndex:0] : nil;
    if (proUpgradeProduct)
    {
        NSLog(@"Product title: %@" , proUpgradeProduct.localizedTitle);
        NSLog(@"Product description: %@" , proUpgradeProduct.localizedDescription);
        NSLog(@"Product price: %@" , proUpgradeProduct.price);
        NSLog(@"Product id: %@" , proUpgradeProduct.productIdentifier);
    }
    
    for (NSString *invalidProductId in response.invalidProductIdentifiers)
    {
        NSLog(@"Invalid product id: %@" , invalidProductId);
    }
}

- (void)showProductDialog
{
    UIAlertView* _alert = [[UIAlertView alloc] init];
    NSString* title = [NSString stringWithFormat:NSLocalizedString(@"iap_title", nil), [HouyiIAPManager getMaxModelCountForFree]];
    [_alert setTitle:title];
    [_alert setMessage:NSLocalizedString(@"iap_content", nil)];
    [_alert setDelegate:self];
    [_alert addButtonWithTitle:NSLocalizedString(@"yes", nil)];
    [_alert addButtonWithTitle:NSLocalizedString(@"no", nil)];
    [_alert show];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    HouyiAppDelegate* delegate = (HouyiAppDelegate*)[UIApplication sharedApplication].delegate;
    if (buttonIndex == 0 && ![delegate isPro]) { // second condition to prevent loop purchase
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
        SKPayment *payment = [SKPayment paymentWithProduct:proUpgradeProduct];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    } else if (buttonIndex == 1) {
        // No do nothing
    }
}

#pragma mark helps
//
// enable pro features
//
- (void)provideContent:(NSString *)productId
{
    if ([productId isEqualToString:kInAppPurchaseProUpgradeProductId])
    {
        // enable the pro features
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"ispro" ];
        [[NSUserDefaults standardUserDefaults] synchronize];
        HouyiAppDelegate* delegate = (HouyiAppDelegate*)[UIApplication sharedApplication].delegate;
        [delegate purchased];
    }
}

//
// removes the transaction from the queue and posts a notification with the transaction result
//
- (void)finishTransaction:(SKPaymentTransaction *)transaction wasSuccessful:(BOOL)wasSuccessful
{
    // remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    
    NSString* msg;
    if (wasSuccessful)
    {
        // send out a notification that we’ve finished the transaction
        NSLog(@"Purchase succeeded");
        msg = NSLocalizedString(@"Purchase Success", nil);
    }
    else
    {
        // send out a notification for the failed transaction
        NSLog(@"Purchase failed");
        msg = NSLocalizedString(@"Purchase Failed", nil);
    }
    
    UIAlertView* _alert = [[UIAlertView alloc] init];
    NSString* title = NSLocalizedString(@"Purchase Status", nil);
    [_alert setTitle:title];
    [_alert setMessage:msg];
    [_alert setDelegate:self];
    [_alert addButtonWithTitle:NSLocalizedString(@"ok", nil)];
    [_alert show];
}
//
// called when the transaction was successful
//
- (void)completeTransaction:(SKPaymentTransaction *)transaction
{
    [self provideContent:transaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

//
// called when a transaction has been restored and and successfully completed
//
- (void)restoreTransaction:(SKPaymentTransaction *)transaction
{
    [self provideContent:transaction.originalTransaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

//
// called when a transaction has failed
//
- (void)failedTransaction:(SKPaymentTransaction *)transaction
{
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
        // error!
        [self finishTransaction:transaction wasSuccessful:NO];
    }
    else
    {
        // this is fine, the user just cancelled, so don’t notify
        [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    }
}

#pragma mark Store Callback
//
// called when the transaction status is updated
//
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
                break;
            default:
                break;
        }
    }
}

@end
