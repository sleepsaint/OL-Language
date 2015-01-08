//
//  main.m
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "OLSource.h"

NSArray* parse_test;
NSArray* lookup_test;
void test_parse() {
    for (NSString* s in parse_test) {
        OLSourceValue* value = [OLSource parse:s];
        if (value) {
            
        }
    }
}
void test_lookup() {
    for (NSString* s in lookup_test) {
        OLSourceValue* value = [OLSource parse:s];
        if (value) {
            
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
        
     //   test_parse();
        PP(test_lookup);
        test_lookup();
    }
    return 0;
}
