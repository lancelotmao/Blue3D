//
//  HouyiIAPManager.h
//  Houyi
//
//  Created by Lance Mao on 22/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@interface HouyiIAPManager : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    SKProduct *proUpgradeProduct;
    SKProductsRequest *productsRequest;
}

+(HouyiIAPManager*)getInstance;
+(int)getMaxModelCountForFree;

- (void)requestProUpgradeProductData;
- (void)showProductDialog;

@end
