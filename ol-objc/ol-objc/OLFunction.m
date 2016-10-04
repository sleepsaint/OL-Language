//
//  OLFunction.m
//  ol-objc
//
//  Created by 伍 威 on 15/1/8.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import "OLFunction.h"
#import "OLSourceValue.h"

static NSMutableDictionary<NSString*, id (^)(NSArray* params, id root, id temp, id now)>* getFunctionTable() {
    static NSMutableDictionary* functionTable;
    if (!functionTable) {
        functionTable = [NSMutableDictionary dictionaryWithDictionary:@{
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
                              return [NSNumber numberWithBool:compareNSObject(params[0], params[1]) > 0];
                          },
                          @">=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:compareNSObject(params[0], params[1]) >= 0];
                          },
                          @"<":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:compareNSObject(params[0], params[1]) < 0];
                          },
                          @"<=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:compareNSObject(params[0], params[1]) <= 0];
                          },
                          @"=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:compareNSObject(params[0], params[1]) == 0];
                          },
                          @"!=":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:compareNSObject(params[0], params[1]) != 0];
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
                              return [NSNumber numberWithBool:(params.count > 0)];
                          },
                          @"or":^(NSArray* params, id root, id temp, id now) {
                              for (id p in params) {
                                  if ([p boolValue]) {
                                      return [NSNumber numberWithBool:YES];
                                  }
                              }
                              return [NSNumber numberWithBool:(params.count > 0)];
                          },
                          @"if":^(NSArray* params, id root, id temp, id now) {
                              return [params[0] boolValue] ? params[1] : params[2];
                          },
                          @"default":^(NSArray* params, id root, id temp, id now) {
                              return [params[0] boolValue] ? params[0] : params[1];
                          },
                          @"filter":^(NSArray* params, id root, id temp, id now) {
                              return filterNSObject(autoLookup(root, temp, now, params[0]), autoLookup(root, temp, now, params[1]), root, temp);
                          },
                          @"sort":^(NSArray* params, id root, id temp, id now) {
                              return sortNSObject(autoLookup(root, temp, now, params[0]), autoLookup(root, temp, now, params[1]), root, temp);
                          },
                          @"some":^(NSArray* params, id root, id temp, id now) {
                              return [NSNumber numberWithBool:someNSObject(autoLookup(root, temp, now, params[0]), autoLookup(root, temp, now, params[1]), root, temp)];
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
                          }];
    }

    return functionTable;
}



@implementation OLFunction

+ (void)set:(NSString*)name function:(id (^)(NSArray* params, id root, id temp, id now))block {
    getFunctionTable()[name] = block;
}

+ (id) calc:(NSString*)name params:(NSArray*)params root:(id)root temp:(id)temp now:(id)now {
    
    id (^function)(NSArray*,id,id,id) = getFunctionTable()[name];
    if (function) {
        return function(params, root, temp, now);
    }
    return nil;
}

@end


NSComparisonResult compareNSObject(id a, id b) {
    if ([a isKindOfClass:[NSString class]]) {
        if ([b isKindOfClass:[NSString class]]) {
            return [(NSString*)a compare:(NSString*)b];
        } else if ([b isKindOfClass:[NSNumber class]]) {
            return [[NSNumber numberWithDouble:[a doubleValue]] compare:(NSNumber*)b];
        } else {
            return NSOrderedSame;
        }
    } else if ([a isKindOfClass:[NSNumber class]]) {
        if ([b isKindOfClass:[NSNumber class]]) {
            return [(NSNumber*)a compare:(NSNumber*)b];
        } else {
            return [(NSNumber*)a compare:[NSNumber numberWithDouble:[b doubleValue]]];
        }
    } else {
        return NSOrderedSame;
    }
}

NSArray* filterNSObject(id obj, id func, id root, id temp) {
    if ([obj isKindOfClass:[NSArray class]]) {
        NSMutableArray* result = [NSMutableArray arrayWithCapacity:[obj count]];
        for (id now in obj) {
            if ([[func lookup:root temp:temp now:now] boolValue]) {
                [result addObject:now];
            }
        }
        return result;
    } else if ([obj isKindOfClass:[NSDictionary class]]) {
        NSMutableArray* result = [NSMutableArray arrayWithCapacity:[obj count]];
        for (id key in obj) {
            id now = [obj valueForKey:key];
            if ([[func lookup:root temp:temp now:now] boolValue]) {
                [result addObject:now];
            }
        }
        return result;
    } else {
        return nil;
    }
}

NSArray* sortNSObject(id obj, id func, id root, id temp) {
    if ([obj isKindOfClass:[NSArray class]]) {
        return [func sort:obj root:root temp:temp];
    } else if ([obj isKindOfClass:[NSDictionary class]]) {
        return [func sort:[obj allValues] root:root temp:temp];
    } else {
        return nil;
    }
}

BOOL someNSObject(id obj, id func, id root, id temp) {
    if ([obj isKindOfClass:[NSArray class]]) {
        for (id now in obj) {
            if ([[func lookup:root temp:temp now:now] boolValue]) {
                return YES;
            }
        }
    } else if ([obj isKindOfClass:[NSDictionary class]]) {
        for (id key in obj) {
            id now = [obj valueForKey:key];
            if ([[func lookup:root temp:temp now:now] boolValue]) {
                return YES;
            }
        }
    }
    return NO;
}

