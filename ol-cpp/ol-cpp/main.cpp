//
//  main.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include <iostream>
#include "oljson.h"
#include "olsource.h"

using namespace std;

void test_parse() {
    vector<string> test = {
        "^.wear.{^.person.{~.person}.wear.hat}.price",
        "#(abd,bdf)",
        "(abd, adfsdf, asdfd, ",
        "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12.4) )",
        "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12.4)",
        "!()",
        "!(abd, !@.123.abd.{@.abd}.)",
        "(abd, !@.123.abd.$123.{@.abd})",
        "(abd, sdfe",
        "(abd, !@.123.abd.123.{@.abd.{~.abd})",
        "(abd, !@.123.abd.123.{@.abd.{.abd})",
        "(abd, !)"
    };
    
    for (const auto& i : test) {
        OL::Source s(i);
        for (auto t = s.getToken(); t.length() > 0; t = s.getToken()) {
            cout << t << endl;
        }
    }
    
}



typedef void (*FUNC)();

void PP(FUNC fun) {
    auto start = clock();
    fun();
    auto end = clock();
    cout <<  end - start << endl;
}

string a = "{\"person\":{\"P0001\":{\"name\":\"Tom\",\"age\":30,\"wear\":{\"hat\":\"W0001\",\"upper\":\"W0002\",\"under\":\"W0003\",\"shoes\":null}},\"P0002\":{\"name\":\"May\",\"age\":25,\"wear\":{\"hat\":\"W0004\",\"upper\":\"W0005\",\"under\":\"W0006\",\"shoes\":\"W0007\"}}},\"wear\":{\"W0001\":{\"name\":\"Red Hat\",\"price\":100},\"W0002\":{\"name\":\"White Jacket\",\"price\":200},\"W0003\":{\"name\":\"Black Shorts\",\"price\":50},\"W0004\":{\"name\":\"White Hat\",\"price\":210},\"W0005\":{\"name\":\"Red Jacket\",\"price\":220},\"W0006\":{\"name\":\"White Skirt\",\"price\":60},\"W0007\":{\"name\":\"Red HHS\",\"price\":10}}}";
char b[] = "{\"person\":{\"P0001\":{\"name\":\"Tom\",\"age\":30,\"wear\":{\"hat\":\"W0001\",\"upper\":\"W0002\",\"under\":\"W0003\",\"shoes\":null}},\"P0002\":{\"name\":\"May\",\"age\":25,\"wear\":{\"hat\":\"W0004\",\"upper\":\"W0005\",\"under\":\"W0006\",\"shoes\":\"W0007\"}}},\"wear\":{\"W0001\":{\"name\":\"Red Hat\",\"price\":100},\"W0002\":{\"name\":\"White Jacket\",\"price\":200},\"W0003\":{\"name\":\"Black Shorts\",\"price\":50},\"W0004\":{\"name\":\"White Hat\",\"price\":210},\"W0005\":{\"name\":\"Red Jacket\",\"price\":220},\"W0006\":{\"name\":\"White Skirt\",\"price\":60},\"W0007\":{\"name\":\"Red HHS\",\"price\":10}}}";
void* getJSON(size_t& length);


void test_json() {
    for (int i = 0; i < 10000; ++i) {
        OL::JSON::parse2(b, sizeof(b));
    }
}

void test_json_parse() {
    size_t length;
    char* s = (char*)getJSON(length);
    OL::Value value = OL::JSON::parse(s, length);
    cout << value.description() << endl;
}

int main(int argc, const char * argv[]) {
  
 //   PP(test_json);
    //    test_parse();
   test_json_parse();
    return 0;
}
