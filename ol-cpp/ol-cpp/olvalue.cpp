//
//  olvalue.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <algorithm>
#include "olvalue.h"
#include "olsource.h"
#include "olfunction.h"

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
    
    ValuePtr autoLookup(ValuePtr call, ValuePtr root, ValuePtr temp, ValuePtr now) {
        ValuePtr ret = call;
        while (typeid(*ret) == typeid(String)) {
            ValuePtr source = Source::parse(ret->description());
            if (source) {
                ret = source->lookup(source, root, temp, now);
            }
        }
        return ret;
    }
    
    ValuePtr Path::lookup(const ValuePtr& call, ValuePtr root, ValuePtr temp, ValuePtr now) {
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
            auto key = k->lookup(k, root, temp, now);
            current = autoLookup(current, root, temp, now);
            if (key && current) {
                current = (*current)[key->description()];
            } else {
                return nullptr;
            }
        }
        return current;
    }
    
    ValuePtr List::lookup(const ValuePtr& call, ValuePtr root, ValuePtr temp, ValuePtr now) {
        ValuePtr name = _head->lookup(_head, root, temp, now);
        if (name) {
            vector<ValuePtr> params;
            params.resize(_tail.size());
            transform(_tail.begin(), _tail.end(), params.begin(), [=](ValuePtr v)->ValuePtr{return v->lookup(v, root, temp, now);});
            return calc(name->description(), params, root, temp, now);
        }
        return nullptr;
    }
    
    ValuePtr Negative::lookup(const ValuePtr& call, ValuePtr root, ValuePtr temp, ValuePtr now) {
        return nullptr;
    }
}