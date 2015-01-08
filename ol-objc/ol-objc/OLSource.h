//
//  OLSource.h
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>

@class OLSourceValue;
@class OLBuffer;

@interface OLSource : NSObject {
    const char* _source;
    const char* _end;
    const char* _cursor;
    int _token;
    enum {
        STRING_TOKEN = 100000,
        NUMBER_TOKEN
    };
    OLBuffer* _tokenString;
    double _tokenNumber;
    NSString* _errorLog;
}

+ (OLSourceValue*) parse:(NSString*)source;
+ (OLSourceValue*) parse:(const char*)source length:(NSUInteger)length;


@end
