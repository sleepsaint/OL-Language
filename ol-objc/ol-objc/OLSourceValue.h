//
//  OLSourceValue.h
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "OLValue.h"

@interface OLSourceValue : NSObject
- (id<OLValue>) lookup:(id<OLValue>)root temp:(id<OLValue>)temp now:(id<OLValue>)now;
@end

@interface OLString : OLSourceValue {
    NSString* _value;
}
- (id<OLValue>) lookup:(id<OLValue>)root temp:(id<OLValue>)temp now:(id<OLValue>)now;
+ (instancetype) value:(NSString*)string;
@end

@interface OLNumber : OLSourceValue {
    double _value;
}
+ (instancetype) value:(double)number;
@end

@interface OLPath : OLSourceValue {
    int _root;
    NSArray* _keys;
}
+ (instancetype) root:(int)root keys:(NSArray*)keys;
@end

@interface OLList : OLSourceValue {
    OLSourceValue* _head;
    NSArray* _tail;
}
+ (instancetype) head:(OLSourceValue*)head tail:(NSArray*)tail;
@end

@interface OLNegative : OLSourceValue {
    OLSourceValue* _value;
}
+ (instancetype) value:(OLSourceValue*)value;
@end

@interface OLQuote : OLSourceValue {
    OLSourceValue* _value;
}
+ (instancetype) value:(OLSourceValue*)value;
@end