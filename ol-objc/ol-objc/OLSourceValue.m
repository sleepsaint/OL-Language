//
//  OLSourceValue.m
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import "OLSourceValue.h"
#import "OLValue.h"
#import "OLSource.h"
#import "OLFunction.h"

@implementation OLSourceValue

- (id) lookup:(id)root temp:(id)temp now:(id)now {
    return nil;
}
- (NSComparisonResult) compare3:(id)root temp:(id)temp a:(id)a b:(id)b {
    return [[self lookup:root temp:temp now:a] compare2:[self lookup:root temp:temp now:b]];
}
@end

id autoLookup(id root, id temp, id now, id current) {
    while ([current isKindOfClass: NSString.class]) {
        OLSourceValue* source = [OLSource parse:current];
        current = [source lookup:root temp:temp now:now];
    }
    return current;
}

@implementation OLString

+ (instancetype) value:(NSString *)string {
    OLString* ret = [[OLString alloc] init];
    ret->_value = string;
    return ret;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now {
    return _value;
}
@end

@implementation OLNumber

+ (instancetype) value:(double)number; {
    OLNumber* ret = [[OLNumber alloc] init];
    ret->_value = [NSNumber numberWithDouble:number];
    return ret;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now {
    return _value;
}

@end


@implementation OLPath

+ (instancetype) root:(int)root keys:(NSArray*)keys {
    OLPath* ret = [[OLPath alloc] init];
    ret->_root = root;
    ret->_keys = keys;
    return ret;
}

- (id) lookup:(id)root temp:(id)temp now:(id)now {
    id current;
    switch (_root) {
        case '^':
            current = root;
            break;
        case '~':
            current = temp;
            break;
        case '@':
            current = now;
            break;
        default:
            return nil;
    }
    for (OLSourceValue* k in _keys) {
        id key = [k lookup:root temp:temp now:now];
        current = autoLookup(root, temp, now, current);
        if (key && current) {
            current = [current getValueByKey:key];
        } else {
            return nil;
        }
    }
    return current;
}
- (NSArray*) sort:(NSArray*)array root:(id)root temp:(id)temp {
    return [array sortedArrayUsingComparator:^(id a, id b){
        return [self compare3:root temp:temp a:a b:b];
    }];
}

@end

@implementation OLList

+ (instancetype) head:(OLSourceValue*)head tail:(NSArray*)tail {
    OLList* ret = [[OLList alloc] init];
    ret->_head = head;
    ret->_tail = tail;
    return ret;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now {
    id name = [_head lookup:root temp:temp now:now];
    if (name) {
        NSMutableArray* params = [NSMutableArray arrayWithCapacity:_tail.count];
        for (OLSourceValue* s in _tail) {
            id value = [s lookup:root temp:temp now:now];
            [params addObject:value?value:[NSNull null]];
        }
        return [OLFunction calc:name params:params root:root temp:temp now:now];
    }
    return nil;
}
- (NSArray*) sort:(NSArray*)array root:(id)root temp:(id)temp {
    return [array sortedArrayUsingComparator:^(id a, id b){
        NSComparisonResult ret = [self->_head compare3:root temp:temp a:a b:b];
        for (OLSourceValue* s in self->_tail) {
            if (ret != NSOrderedSame) {
                return ret;
            } else {
                ret = [s compare3:root temp:temp a:a b:b];
            }
        }
        return ret;
    }];
}
@end

@implementation OLNegative

+ (instancetype) value:(OLSourceValue*)value {
    OLNegative* ret = [[OLNegative alloc] init];
    ret->_value = value;
    return ret;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now {
    return [NSNumber numberWithBool:![[_value lookup:root temp:temp now:now] boolValue]];
}
- (NSArray*) sort:(NSArray*)array root:(id)root temp:(id)temp {
    return [array sortedArrayUsingComparator:^(id a, id b){
        return -[self->_value compare3:root temp:temp a:a b:b];
    }];
}

@end

@implementation OLQuote

+ (instancetype) value:(OLSourceValue*)value {
    OLQuote* ret = [[OLQuote alloc] init];
    ret->_value = value;
    return ret;
}
- (id) lookup:(id)root temp:(id)temp now:(id)now {
    return _value;
}

@end