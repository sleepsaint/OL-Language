//
//  web.m
//  ol-cpp
//
//  Created by 伍威 on 15/1/3.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>

void* getJSON(size_t& length) {
    NSData* s = [NSData dataWithContentsOfURL:[NSURL URLWithString:@"http://localhost/a.php"]];
    length = s.length;
    void* ret = malloc(length);
    [s getBytes:ret length:length];
    return ret;
}
