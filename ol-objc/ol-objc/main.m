//
//  main.m
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "OLSource.h"
#import "OLSourceValue.h"
NSArray* parse_test;
NSArray* lookup_test;
id root_json;
id temp_json;

void test_parse() {
    for (NSString* s in parse_test) {
        OLSourceValue* value = [OLSource parse:s];
        if (value) {
            
        }
    }
}
void test_lookup() {
    for (NSString* s in lookup_test) {
        id value = [[OLSource parse:s] lookup:root_json temp:temp_json now:root_json];
        if (value) {
            printf("%s\n", s.UTF8String);
            printf("%s\n", [value stringValue].UTF8String);
        }
    }
}
typedef void (*FUNC)();

void PP(FUNC func) {
    clock_t start = clock();
    for (int i = 0; i < 1000; ++i) {
        func();
    }
    clock_t end = clock();
    printf("%lu\n", end - start);
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        parse_test = @[
                        @"^.wear.{^.person.{~.person}.wear.hat}.price",
                        @"#(abd,bdf)",
                        @"(abd, adfsdf, asdfd, ",
                        @"!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12.4) )",
                        @"!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12)",
                        @"!()",
                        @"!(abd, !@.123.abd.{@.abd}.)",
                        @"(abd, !@.123.abd.$123.{@.abd})",
                        @"(abd, sdfe",
                        @"(abd, !@.123.abd.123.{@.abd.{~.abd})",
                        @"(abd, !@.123.abd.123.{@.abd.{.abd})",
                        @"(abd, !)"
                        ];
        
        lookup_test = @[
                        @"^.wear.{^.person.{~.person}.wear.hat}.price",
                        @"^.wear.{~.person2.wear.hat}.price",
                        @"~.personwear.price",
                        @"(-, (+, ^.wear.W0001.price, ^.wear.W0002.price), ^.wear.W0002.price)",
                        @"(filter, ^.wear, #(>, @.price, $150))",
                        @"(filter, ^.wear, ~.wearfilter1)",
                        @"(or, !(>,^.wear.W0001.price,50), (>, ^.wear.W0002.price, 100) )",
                        @"(sort, ^.wear, #(@.price))",
                        @"(sort, ^.wear, #(!(=,@.name,Red Hat),@.price))",
                        @"(sort, ^.wear, ~.wearsorter1)",
                        @"(some, ^.wear, ~.wearfilter1)",
                        @"(sort, ~.now, ~.wearsorter1)",
                        @"(some, ~.now, #(>,@.price,50))",
                        @"(sort, ^.wear, #@.price)",
                        @"(sort, ^.wear, #!@.price)",
                        @"(random)",
                        @"(random, $5)",
                        @"(random, $-5, $-3)"
                        ];
        root_json = [NSJSONSerialization JSONObjectWithData:[@"{\"person\":{\"P0001\":{\"name\":\"Tom\",\"age\":30,\"wear\":{\"hat\":\"W0001\",\"upper\":\"W0002\",\"under\":\"W0003\",\"shoes\":null}},\"P0002\":{\"name\":\"May\",\"age\":25,\"wear\":{\"hat\":\"W0004\",\"upper\":\"W0005\",\"under\":\"W0006\",\"shoes\":\"W0007\"}}},\"wear\":{\"W0001\":{\"name\":\"Red Hat\",\"price\":100},\"W0002\":{\"name\":\"White Jacket\",\"price\":200},\"W0003\":{\"name\":\"Black Shorts\",\"price\":50},\"W0004\":{\"name\":\"White Hat\",\"price\":210},\"W0005\":{\"name\":\"Red Jacket\",\"price\":220},\"W0006\":{\"name\":\"White Skirt\",\"price\":60},\"W0007\":{\"name\":\"Red HHS\",\"price\":10}}}" dataUsingEncoding:NSUTF8StringEncoding] options: 0 error: nil];
        temp_json = [NSJSONSerialization JSONObjectWithData:[@"{\"person\":\"P0001\",\"person2\":\"^.person.P0001\",\"wearnow\":\"upper\",\"personwear\":\"^.wear.{~.person2.wear.{~.wearnow}}\",\"wearfilter1\":\"#(>, @.price, $150)\",\"wearsorter1\":\"#(!(=,@.name,Red Hat),!@.price))\",\"now\":\"^.wear\"}" dataUsingEncoding:NSUTF8StringEncoding] options: 0 error: nil];
     //   test_parse();
//        PP(test_lookup);
        test_lookup();
    }
    return 0;
}
