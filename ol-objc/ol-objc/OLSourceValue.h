//
//  OLSourceValue.h
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface OLSourceValue : NSObject
- (id) lookup:(id)root temp:(id)temp now:(id)now;
- (NSComparisonResult) compare3:(id)root temp:(id)temp a:(id)a b:(id)b;
- (void)change:(id)root temp:(id)temp now:(id)now toValue:(id)toValue;
@end

@interface OLString : OLSourceValue {
    NSString* _value;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now;
+ (instancetype) value:(NSString*)string;
@end

@interface OLNumber : OLSourceValue {
    NSNumber* _value;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now;
+ (instancetype) value:(double)number;
@end

@interface OLPath : OLSourceValue {
    int _root;
    NSArray* _keys;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now;
- (NSArray*) sort:(NSArray*)array root:(id)root temp:(id)temp;
+ (instancetype) root:(int)root keys:(NSArray*)keys;
@end

@interface OLList : OLSourceValue {
    OLSourceValue* _head;
    NSArray* _tail;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now;
- (NSArray*) sort:(NSArray*)array root:(id)root temp:(id)temp;
+ (instancetype) head:(OLSourceValue*)head tail:(NSArray*)tail;
@end

@interface OLNegative : OLSourceValue {
    OLSourceValue* _value;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now;
- (NSArray*) sort:(NSArray*)array root:(id)root temp:(id)temp;
+ (instancetype) value:(OLSourceValue*)value;
@end

@interface OLQuote : OLSourceValue {
    OLSourceValue* _value;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now;
+ (instancetype) value:(OLSourceValue*)value;
@end

id autoLookup(id root, id temp, id now, id current);
