//
//  OLValue.h
//  ol-objc
//
//  Created by 伍威 on 15/1/8.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (OLValue)
- (id) getValueByKey:(id)key;
@end

@interface NSNumber (OLValue)
- (id) getValueByKey:(id)key;
@end

@interface NSArray (OLValue)
- (id) getValueByKey:(id)key;
@end

@interface NSDictionary (OLValue)
- (id) getValueByKey:(id)key;
@end