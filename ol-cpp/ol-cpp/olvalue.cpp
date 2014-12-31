//
//  olvalue.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include <sstream>
#include "olvalue.h"

using namespace std;

namespace OL {
    Number::Number(double value) : _value(value) {
        
    }
    
    string Number::description() {
        if ((int)_value == _value) {
            return to_string((int)_value);
        } else {
            return to_string(_value);
        }
    }
    
    string String::description() {
        return _value;
    }
    
    string Array::description() {
        ostringstream s;
        s << "[";
        auto i = _value.begin();
        if (i != _value.end()) {
            s << (*i)->description();
            ++i;
            while (i != _value.end()) {
                s << "," << (*i)->description();
                ++i;
            }
        }
        s << "]";
        return s.str();
    }
    
    string Dictionary::description() {
        ostringstream s;
        s << "{";
        auto i = _value.begin();
        if (i != _value.end()) {
            s << i->first << ":" << i->second->description();
            ++i;
            while (i != _value.end()) {
                s << "," << i->first << ":" << i->second->description();
                ++i;
            }
        }
        s << "}";
        return s.str();
    }
    
    string Null::description() {
        return "null";
    }
    
}