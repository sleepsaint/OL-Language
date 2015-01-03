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
    
    Value::Value() : _type(Null) {}
    
    Value::Value(Value&& value) :_type(value._type), _array(value._array) {
        value._type = Null;
    }
    
    Value::~Value() {
        switch (_type) {
            case String:
                delete _string;
                break;
            case Array:
                delete _array;
                break;
            case Object:
                delete _object;
                break;
            case Path:
                delete _path;
                break;
            case List:
                delete _path;
                break;
            case Negative:
            case Quote:
                delete _value;
                break;
            default:
                ;
        }
        _type = Null;
    }
    
    
    string Value::descriptionNumber() {
        if ((int)_number == _number) {
            return to_string((int)_number);
        } else {
            return to_string(_number);
        }
    }
    
    string Value::descriptionString() {
        return *_string;
    }
    
    string Value::descriptionArray() {
        ostringstream s;
        s << "[";
        auto i = _array->begin();
        if (i != _array->end()) {
            s << i->description();
            ++i;
            while (i != _array->end()) {
                s << "," << i->description();
                ++i;
            }
        }
        s << "]";
        return s.str();
    }
    
    string Value::descriptionObject() {
        ostringstream s;
        s << "{";
        auto i = _object->begin();
        if (i != _object->end()) {
            s << i->first << ":" << i->second.description();
            ++i;
            while (i != _object->end()) {
                s << "," << i->first << ":" << i->second.description();
                ++i;
            }
        }        s << "}";
        return s.str();
    }
    
    string Value::descriptionNull() {
        return "null";
    }
    
    string Value::descriptionPath() {
        ostringstream s;
        s << _path->first.description();
        for (auto& i : _path->second) {
            s << "[" << i.description() << "]";
        }
        return s.str();
    }
    
    string Value::descriptionList() {
        ostringstream s;
        s << _path->first.description() << "(";
        auto& tail = _path->second;
        auto i = tail.begin();
        if (i != tail.end()) {
            s << i->description();
            ++i;
            while (i != tail.end()) {
                s << "," << i->description();
                ++i;
            }
        }
        s << ")";
        return s.str();
    }
    
    string Value::descriptionNegative() {
        return "!" + _value->description();
    }
    
    string Value::descriptionQuote() {
        return "#" + _value->description();
    }
    
    string Value::descriptionBool() {
        return _bool ? "true" : "false";
    }
    
    string Value::description() {
        switch (_type) {
            case Null:
                return descriptionNull();
            case Number:
                return descriptionNumber();
            case String:
                return descriptionString();
            case Array:
                return descriptionArray();
            case Object:
                return descriptionObject();
            case Path:
                return descriptionPath();
            case List:
                return descriptionList();
            case Negative:
                return descriptionNegative();
            case Quote:
                return descriptionQuote();
            case Bool:
                return descriptionBool();
            default:
                return "";
        }
    }
    
}