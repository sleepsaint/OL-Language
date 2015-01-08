//
//  OLSourceValue.m
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import "OLSourceValue.h"

@implementation OLSourceValue
- (id<OLValue>) lookup:(id<OLValue>)root temp:(id<OLValue>)temp now:(id<OLValue>)now {
    return nil;
}
@end

@implementation OLString

+ (instancetype) value:(NSString *)string {
    OLString* ret = [[OLString alloc] init];
    ret->_value = string;
    return ret;
}
- (id<OLValue>) lookup:(id<OLValue>)root temp:(id<OLValue>)temp now:(id<OLValue>)now {
    return nil;
}
@end

@implementation OLNumber

+ (instancetype) value:(double)number; {
    OLNumber* ret = [[OLNumber alloc] init];
    ret->_value = number;
    return ret;
}

@end


@implementation OLPath

+ (instancetype) root:(int)root keys:(NSArray*)keys {
    OLPath* ret = [[OLPath alloc] init];
    ret->_root = root;
    ret->_keys = keys;
    return ret;
}

@end

@implementation OLList

+ (instancetype) head:(OLSourceValue*)head tail:(NSArray*)tail {
    OLList* ret = [[OLList alloc] init];
    ret->_head = head;
    ret->_tail = tail;
    return ret;
}

@end

@implementation OLNegative

+ (instancetype) value:(OLSourceValue*)value {
    OLNegative* ret = [[OLNegative alloc] init];
    ret->_value = value;
    return ret;
}

@end

@implementation OLQuote

+ (instancetype) value:(OLSourceValue*)value {
    OLQuote* ret = [[OLQuote alloc] init];
    ret->_value = value;
    return ret;
}

@end