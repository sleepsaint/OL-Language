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
using namespace OL;

vector<string> test2 = {
    "^.wear.{^.person.{~.person}.wear.hat}.price",
    "`(abd,bdf)",
    "(abd, adfsdf, asdfd, ",
    "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12.4) )",
    "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12)",
    "!()",
    "!(abd, !@.123.abd.{@.abd}.)",
    "(abd, !@.123.abd.$123.{@.abd})",
    "(abd, sdfe",
    "(abd, !@.123.abd.123.{@.abd.{~.abd})",
    "(abd, !@.123.abd.123.{@.abd.{.abd})",
    "(abd, !)"
};


void test_parse() {
    
    for (const auto& i : test2) {
        auto value = OL::Source::parse(i);
        cout << value.description() << endl;
    }
    
}
void test_parse2() {
    
    for (const auto& i : test2) {
        OL::Source::parse(i.c_str(), i.length());
    }
    
}
string root = "{\"person\":{\"P0001\":{\"name\":\"Tom\",\"age\":30,\"wear\":{\"hat\":\"W0001\",\"upper\":\"W0002\",\"under\":\"W0003\",\"shoes\":null}},\"P0002\":{\"name\":\"May\",\"age\":25,\"wear\":{\"hat\":\"W0004\",\"upper\":\"W0005\",\"under\":\"W0006\",\"shoes\":\"W0007\"}}},\"wear\":{\"W0001\":{\"name\":\"Red Hat\",\"price\":100},\"W0002\":{\"name\":\"White Jacket\",\"price\":200},\"W0003\":{\"name\":\"Black Shorts\",\"price\":50},\"W0004\":{\"name\":\"White Hat\",\"price\":210},\"W0005\":{\"name\":\"Red Jacket\",\"price\":220},\"W0006\":{\"name\":\"White Skirt\",\"price\":60},\"W0007\":{\"name\":\"Red HHS\",\"price\":10}},\"book\":[{\"name\":\"book001\"},{\"name\":\"book002\"}]}";

string temp = "{\"person\":\"P0001\",\"person2\":\"^.person.P0001\",\"wearnow\":\"upper\",\"personwear\":\"^.wear.{~.person2.wear.{~.wearnow}}\",\"wearfilter1\":\"`(>, @.price, $150)\",\"wearsorter1\":\"`(!(=,@.name,Red Hat),!@.price))\",\"now\":\"^.wear\"}";
vector<string> test = {
    "^.book.0",
    "^.book.1.name",
    "^.wear.{^.person.{~.person}.wear.hat}.price",
    "^.wear.{~.person2.wear.hat}.price",
    "~.personwear.price",
    "(-, (+, ^.wear.W0001.price, ^.wear.W0002.price), ^.wear.W0002.price)",
    "(*, (+, ^.wear.W0001.price, ^.wear.W0002.price), ^.wear.W0002.price)",
    "(filter, ^.wear, `(>, @.price, $150))",
    "(filter, ^.wear, ~.wearfilter1)",
    "(or, !(>,^.wear.W0001.price,50), (>, ^.wear.W0002.price, 100) )",
    "(sort, ^.wear, `(@.price))",
    "(sort, ^.wear, `(!(=,@.name,Red Hat),@.price))",
    "(sort, ^.wear, ~.wearsorter1)",
    "(some, ^.wear, ~.wearfilter1)",
    "(sort, ~.now, ~.wearsorter1)",
    "(some, ^.wear, `(>,@.price,$50))",
    "(sort, ^.wear, `@.price)",
    "(sort, ^.wear, `!@.price)",
    "(random)",
    "(random, $5)",
    "(random, $-5, $-3)",
    "(if, (>, $3, $4), 3>4, 3<=4)",
    "(default,$0,1)",
    "(default,5,2)",
};

vector<pair<string, OL::Value>> change_test = {
    {"^.wear.{^.person.{~.person}.wear.hat}.price", 500},
    {"^.book.1.name", "abcde"},
    {"^.book", map<string, OL::Value>({{"name", "abcd"}})}
};

vector<string> remove_test = {
    "^.wear.{^.person.{~.person}.wear.hat}.price"
};

auto root_json = OL::JSON::parse(root);
auto temp_json = OL::JSON::parse(temp);

void test_lookup2() {
    using namespace OL;
     for (const auto& i : test) {
        auto value = Source::parse(i);
        value.lookup(root_json, temp_json, root_json);
    }
   
}
void test_lookup() {
    using namespace OL;
    for (const auto& i : test) {
        auto value = Source::parse(i);
        auto value2 = value.lookup(root_json, temp_json, root_json);
        cout << i << endl;
        cout << value2.description() << endl;
    }
    
}
void test_change() {
    using namespace OL;
    for (const auto& i : change_test) {
        auto source = Source::parse(i.first);
        auto value = source.lookup(root_json, temp_json, root_json);
        cout << i.first << " " << i.second.description() << endl;
        cout << value.description() << endl;
        source.change(root_json, temp_json, root_json, i.second);
        cout << source.lookup(root_json, temp_json, root_json).description() << endl;
    }
    
}
void test_remove() {
    using namespace OL;
    for (const auto& i : remove_test) {
        auto source = Source::parse(i);
        auto value = source.lookup(root_json, temp_json, root_json);
        if (value) {
            cout << i << endl;
            cout << value.description() << endl;
            source.remove(root_json, temp_json, root_json);
            value = source.lookup(root_json, temp_json, root_json);
            if (value) {
                cout << value.description() << endl;
            } else {
                cout << "null" << endl;
            }
            
        }
    }
    
    auto source = Source::parse("^.book");
    cout << source.lookup(root_json, temp_json, root_json).description() << endl;
    auto remove = Source::parse("^.book.0");
    remove.remove(root_json, temp_json, root_json);
    cout << source.lookup(root_json, temp_json, root_json).description() << endl;

}

void test_parse_json() {
    auto json = OL::JSON::parse(root);
    cout << json.description() << endl;
}
void test_parse_json2() {
    OL::JSON::parse(root);

}
typedef void (*FUNC)();

void PP(FUNC func) {
    auto start = clock();
    for (int i = 0; i < 1000; ++i) {
        func();
    }
    auto end = clock();
    cout << end - start << endl;
}

void test_json_op() {
    root_json["book"][1] = "abde";
//    root_json["abc"] = 5;
//    root_json["book"] = ArrayValue({"abde","bbde"});
//    root_json["book"] = ObjectValue({{"abde","bbde"}});
//    root_json["book"] = "abcd";
//    cout << root_json["book"].description() << endl;
//    cout << root_json.description() << endl;

    
    
    for (const auto& i : root_json) {
        for (const auto& j : i.value()) {
            cout << j.description() << endl;
        }
    }

}

int main(int argc, const char * argv[]) {
//    OL::Source::debug = true;
//    test_parse();
//    PP(test_parse2);
//    test_parse_json();
//    PP(test_parse_json2);
//    PP(test_lookup2);
//    test_lookup();
//    test_change();
//    test_remove();
    test_json_op();
//    getchar();
    return 0;
}
