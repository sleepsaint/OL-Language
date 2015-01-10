//
//  main.c
//  ol-c
//
//  Created by 伍 威 on 15/1/9.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "oljson.h"

const char* rootString = "{\"person\":{\"P0001\":{\"name\":\"Tom\",\"age\":30,\"wear\":{\"hat\":\"W0001\",\"upper\":\"W0002\",\"under\":\"W0003\",\"shoes\":null}},\"P0002\":{\"name\":\"May\",\"age\":25,\"wear\":{\"hat\":\"W0004\",\"upper\":\"W0005\",\"under\":\"W0006\",\"shoes\":\"W0007\"}}},\"wear\":{\"W0001\":{\"name\":\"Red Hat\",\"price\":100},\"W0002\":{\"name\":\"White Jacket\",\"price\":200},\"W0003\":{\"name\":\"Black Shorts\",\"price\":50},\"W0004\":{\"name\":\"White Hat\",\"price\":210},\"W0005\":{\"name\":\"Red Jacket\",\"price\":220},\"W0006\":{\"name\":\"White Skirt\",\"price\":60},\"W0007\":{\"name\":\"Red HHS\",\"price\":10}}}";

const char* tempString =  "{\"person\":\"P0001\",\"person2\":\"^.person.P0001\",\"wearnow\":\"upper\",\"personwear\":\"^.wear.{~.person2.wear.{~.wearnow}}\",\"wearfilter1\":\"#(>, @.price, $150)\",\"wearsorter1\":\"#(!(=,@.name,Red Hat),!@.price))\",\"now\":\"^.wear\"}";

size_t length;
typedef void (*FUNC)();

void PP(FUNC func) {
    clock_t start = clock();
    for (int i = 0; i < 1000; ++i) {
        func();
    }
    clock_t end = clock();
    printf("%ld\n", end - start);
}

void test_parse_json2() {
    OLValue value = OLParseJSON(rootString, length);
    OLValueCleanup(&value);
}

void test_parse_json() {
    OLValue value = OLParseJSON(rootString, length);
    OLValuePrint(value);
}

int main(int argc, const char * argv[]) {
    length = strlen(rootString);
    PP(test_parse_json2);
    return 0;
}
