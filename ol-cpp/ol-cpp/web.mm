//
//  web.m
//  ol-cpp
//
//  Created by 伍威 on 15/1/3.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <string>

std::string getJSON() {
    NSData* s = [NSData dataWithContentsOfURL:[NSURL URLWithString:@"http://localhost/a.php"]];
    return std::string((char*)s.bytes, s.length);
}
