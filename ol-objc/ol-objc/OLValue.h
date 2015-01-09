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
- (NSComparisonResult) compare2:(id)other;
@end

@interface NSNumber (OLValue)
- (id) getValueByKey:(id)key;
- (NSComparisonResult) compare2:(id)other;
@end

@interface NSArray (OLValue)
- (id) getValueByKey:(id)key;
- (id) filter:(id)function root:(id)root temp:(id)temp;
- (NSArray*) arrayValue;
@end

@interface NSDictionary (OLValue)
- (id) getValueByKey:(id)key;
- (id) filter:(id)function root:(id)root temp:(id)temp;
- (NSArray*) arrayValue;
@end

@interface NSNull (OLValue)
- (NSString*) stringValue;
- (id) getValueByKey:(id)key;
@end