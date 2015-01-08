//
//  OLBuffer.m
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import "OLBuffer.h"

@implementation OLBuffer

- (instancetype) init {
    if (self = [super init]) {
        _begin = _static;
        _cursor = _static;
        _end = _static + STATIC_MAX_LENGTH - 1;
    }
    return self;
}

- (void) append:(char)c {
    if (_cursor < _end) {
        *_cursor = c;
        ++_cursor;
    }
}

- (void) reset {
    _begin = _static;
    _cursor = _static;
    _end = _static + STATIC_MAX_LENGTH;
}

- (NSString*) string {
    *_cursor = 0;
    return [NSString stringWithCString:_begin encoding:NSUTF8StringEncoding];
}

@end
