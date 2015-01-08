//
//  OLBuffer.h
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface OLBuffer : NSObject {
    enum { STATIC_MAX_LENGTH = 8192 };
    char _static[STATIC_MAX_LENGTH];
    char* _begin;
    char* _cursor;
    char* _end;
}

- (void) append:(char)c;
- (void) reset;
- (NSString*) string;

@end
