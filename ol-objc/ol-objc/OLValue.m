//
//  OLValue.m
//  ol-objc
//
//  Created by 伍 威 on 15/1/8.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "OLValue.h"
#import "OLSourceValue.h"

@implementation NSString (OLValue)

- (id) getValueByKey:(id)key {
    return nil;
}
- (NSComparisonResult) compare2:(id)other {
    if ([other isKindOfClass:NSString.class]) {
        return [self compare:other];
    } else if ([other isKindOfClass:NSNumber.class]) {
        return [[NSNumber numberWithDouble:self.doubleValue] compare:other];
    } else {
        return 0;
    }
}
@end

@implementation NSNumber (OLValue)

- (id) getValueByKey:(id)key {
    return nil;
}
- (NSComparisonResult) compare2:(id)other {
    if ([other isKindOfClass:NSNumber.class]) {
        return [self compare:other];
    } else if ([other isKindOfClass:NSString.class]) {
        return [self compare:[NSNumber numberWithDouble:[other doubleValue]]];
    } else {
        return 0;
    }
}
@end

@implementation NSArray (OLValue)

- (id) getValueByKey:(id)key {
    return [self objectAtIndex:[key integerValue]];
}
- (id) filter:(id)function root:(id)root temp:(id)temp {
    NSMutableArray* ret = [NSMutableArray arrayWithCapacity:self.count];
    for (id now in self) {
        if ([function lookup:root temp:temp now:now]) {
            [ret addObject:now];
        }
    }
    return ret;
}
- (NSArray*) arrayValue {
    return self;
}
@end

@implementation NSDictionary (OLValue)

- (id) getValueByKey:(id)key {
    return [self objectForKey:key];
}
- (id) filter:(id)function root:(id)root temp:(id)temp {
    NSMutableDictionary* ret = [NSMutableDictionary dictionaryWithCapacity:self.count];
    for (id key in self) {
        if ([[function lookup:root temp:temp now:[self objectForKey:key]] boolValue]) {
            ret[key] = [self objectForKey:key];
        }
    }
    return ret;
}
- (NSArray*) arrayValue {
    return self.allValues;
}
@end

@implementation NSNull (OLValue)

- (id) getValueByKey:(id)key {
    return nil;
}
- (NSString*) stringValue {
    return @"null";
}
- (id) filter:(id)function root:(id)root temp:(id)temp {
    return nil;
}
@end