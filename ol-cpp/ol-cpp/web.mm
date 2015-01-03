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
    NSString* s = [NSString stringWithContentsOfURL:[NSURL URLWithString:@"http://localhost/a.php"] encoding:NSUTF8StringEncoding error:nil];
    return std::string(s.UTF8String, s.length);
}
