//
//  OLFunction.m
//  ol-objc
//
//  Created by 伍 威 on 15/1/8.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import "OLFunction.h"
#import "OLValue.h"
#import "OLSourceValue.h"

static NSDictionary* FunctionTable;

@implementation OLFunction

+ (id) calc:(id)name params:(NSArray*)params root:(id)root temp:(id)temp now:(id)now {
    
    if (!FunctionTable) {
        FunctionTable = @{
                          @"+":^(NSArray* params, id root, id temp, id now) {
                              double ret = 0;
                              for (id p in params) {
                                  ret += [p doubleValue];
                              }
                              return [NSNumber numberWithDouble:ret];
                          },
                          @"-":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithDouble:[params[0] doubleValue] - [params[1] doubleValue]];
                          },
                          @"*":^(NSArray* params, id root, id temp, id now) {
                              double ret = 1;
                              for (id p in params) {
                                  ret *= [p doubleValue];
                              }
                              return [NSNumber numberWithDouble:(params.count > 0 ? ret: 0)];
                          },
                          @"/":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithDouble:[params[0] doubleValue] / [params[1] doubleValue]];
                          },
                          @">":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:[params[0] compare2:params[1]] > 0];
                          },
                          @">=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:[params[0] compare2:params[1]] >= 0];
                          },
                          @"<":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:[params[0] compare2:params[1]] < 0];
                          },
                          @"<=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:[params[0] compare2:params[1]] <= 0];
                          },
                          @"=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:[params[0] compare2:params[1]] == 0];
                          },
                          @"!=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:[params[0] compare2:params[1]] != 0];
                          },
                          @"not":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:![params[0] boolValue]];
                          },
                          @"and":^(NSArray* params, id root, id temp, id now) {
                              for (id p in params) {
                                  if (![p boolValue]) {
                                      return [NSNumber numberWithBool:NO];
                                  }
                              }
                              return [NSNumber numberWithBool:!params.count > 0];
                          },
                          @"or":^(NSArray* params, id root, id temp, id now) {
                              for (id p in params) {
                                  if ([p boolValue]) {
                                      return [NSNumber numberWithBool:YES];
                                  }
                              }
                              return [NSNumber numberWithBool:!NO];
                          },
                          @"if":^(NSArray* params, id root, id temp, id now) {
                              return [params[0] boolValue] ? params[1] : params[2];
                          },
                          @"filter":^(NSArray* params, id root, id temp, id now) {
                              return [autoLookup(root, temp, now, params[0]) filter:autoLookup(root, temp, now, params[1]) root:root temp:temp];
                          },
                          @"sort":^(NSArray* params, id root, id temp, id now) {
                              NSArray* array = [autoLookup(root, temp, now, params[0]) arrayValue];
                              return [autoLookup(root, temp, now, params[1]) sort:array root:root temp:temp];
                          },
                          @"some":^(NSArray* params, id root, id temp, id now) {
                              return [autoLookup(root, temp, now, params[0]) some:autoLookup(root, temp, now, params[1]) root:root temp:temp];
                          },
                          @"random":^(NSArray* params, id root, id temp, id now) {
                              double ret = (double)arc4random() / (double)UINT32_MAX;
                              switch (params.count) {
                                  case 0:
                                      break;
                                  case 1:
                                      ret = floor(ret * [params[0] doubleValue]);
                                      break;
                                  default:
                                      ret = [params[0] doubleValue] + floor(ret * ([params[1] doubleValue] - [params[0] doubleValue]));
                                      break;
                              }
                              return [NSNumber numberWithDouble:ret];
                          }
                          };
    }
    id (^function)(NSArray*,id,id,id) = FunctionTable[name];
    if (function) {
        return function(params, root, temp, now);
    }
    return nil;
}

@end
