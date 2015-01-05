//
//  olvalue.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "olvalue.h"
#include "olsource.h"

using namespace std;

namespace OL {
    
    
    string Number::description() {
        if ((int)_value == _value) {
            return to_string((int)_value);
        } else {
            return to_string(_value);
        }
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
    
    string Object::description() {
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
    
    string Path::description() {
        ostringstream s;
        s << _root;
        for (auto& i : _keys) {
            s << "[" << i->description() << "]";
        }
        return s.str();
    }
    
    string List::description() {
        ostringstream s;
        s << _head->description() << "(";
        auto i = _tail.begin();
        if (i != _tail.end()) {
            s << (*i)->description();
            ++i;
            while (i != _tail.end()) {
                s << "," << (*i)->description();
                ++i;
            }
        }
        s << ")";
        return s.str();
    }
    
    string Bool::description() {
        return _value ? "true" : "false";
    }
    
    ValuePtr Path::lookup(ValuePtr root, ValuePtr temp, ValuePtr now) {
        ValuePtr current;
        switch (_root) {
            case '^':
                current = root;
                break;
            case '~':
                current = temp;
                break;
            case '@':
                current = now;
                break;
            default:
                return nullptr;
        }
        for (auto& k : _keys) {
            auto key = k->lookup(root, temp, now);
            if (key && current) {
//                current = (*current)[key->description()];
            } else {
                return nullptr;
            }
        }
        return nullptr;
    }
    
    ValuePtr List::lookup(ValuePtr root, ValuePtr temp, ValuePtr now) {
        return nullptr;
    }
    
    ValuePtr Negative::lookup(ValuePtr root, ValuePtr temp, ValuePtr now) {
        return nullptr;
    }
}