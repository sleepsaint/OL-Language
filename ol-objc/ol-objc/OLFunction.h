//
//  OLFunction.h
//  ol-objc
//
//  Created by 伍 威 on 15/1/8.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface OLFunction : NSObject
+ (id) calc:(id)name params:(NSArray*)params root:(id)root temp:(id)temp now:(id)now;
@end
