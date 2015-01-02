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
    
    Path::Path(Value* root) : _root(root) {
        
    }
    
    void Path::addKey(Value *key) {
        _keys.push_back(ValuePtr(key));
    }
    
    string Path::description() {
        ostringstream s;
        s << _root->description();
        for (const auto& i : _keys) {
            s << "[" << i->description() << "]";
        }
        return s.str();
    }
    
    List::List(Value* head) : _head(head) {
        
    }
    
    void List::addItem(Value *item) {
        _tail.push_back(ValuePtr(item));
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
    
    Negative::Negative(Value* value) : _value(value) {
        
    }
    
    string Negative::description() {
        return "!" + _value->description();
    }
    
    Quote::Quote(Value* value) : _value(value) {
        
    }
    
    string Quote::description() {
        return "#" + _value->description();
    }
    
}