//
//  OLValue.m
//  ol-objc
//
//  Created by 伍 威 on 15/1/8.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "OLValue.h"

@implementation NSString (OLValue)

- (id) getValueByKey:(id)key {
    return nil;
}

@end

@implementation NSNumber (OLValue)

- (id) getValueByKey:(id)key {
    return nil;
}

@end

@implementation NSArray (OLValue)

- (id) getValueByKey:(id)key {
    return [self objectAtIndex:[key integerValue]];
}

@end

@implementation NSDictionary (OLValue)

- (id) getValueByKey:(id)key {
    return [self objectForKey:key];
}

@end