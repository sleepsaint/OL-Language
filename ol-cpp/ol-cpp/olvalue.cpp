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

using namespace std;

namespace OL {
    
    Value::Value() : _type(Null) {}
    
    Value::Value(Value&& value) :_type(value._type), _array(value._array) {
        value._type = Null;
    }
    
    Value::Value(Value* pointer) : _type(Pointer), _value(pointer) {}
    
    Value::Value(bool b) : _type(Bool), _bool(b) {}
    
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
            case List:
                delete _pair;
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
    
    string Value::descriptionChar() {
        return string(1, _char);
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
        s << _pair->first.description();
        for (auto& i : _pair->second) {
            s << "[" << i.description() << "]";
        }
        return s.str();
    }
    
    string Value::descriptionList() {
        ostringstream s;
        s << _pair->first.description() << "(";
        auto& tail = _pair->second;
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
            case Char:
                return descriptionChar();
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
            case Pointer:
                return _value->description();
            default:
                return "";
        }
    }
    
    Value::operator bool () {
        switch (_type) {
            case Null:
                return false;
            case Char:
                return _char != 0;
            case Number:
                return _number != 0;
            case String:
                return !_string->empty();
            case Array:
                return !_array->empty();
            case Object:
                return !_object->empty();
            case Path:
            case List:
            case Negative:
            case Quote:
                return true;
            case Bool:
                return _bool;
            default:
                return false;
        }
        
    }
    
    static Value NullValue;
    
    Value& Value::at(const Value& key) {
        Value* container = this->_type == Pointer ? _value : this;
        const Value* index = key._type == Pointer ? key._value : &key;
        switch (container->_type) {
            case Array:
                switch (index->_type) {
                    case Number:
                        if ( index->_number < container->_array->size()) {
                            return container->_array->at(index->_number);
                        }
                    default:
                        break;
                }
                break;
            case Object:
                switch (index->_type) {
                    case String:
                        return container->_object->at(*index->_string);
 
                    default:
                        break;
                }
                break;
            default:
                break;
        }
        return NullValue;
    }
    
    Value Value::lookup(Value& root, Value& temp, Value& now) {
        switch (_type) {
            case Null:
            case Char:
            case Number:
            case Bool:
            case String:
            case Array:
            case Object:
                return this;
            case Path:
                return lookupPath(root, temp, now);
                //            case List:
                //                return descriptionList();
            case Negative:
                return !(*this);
            case Quote:
                return _value;
            default:
                return Value();
        }
    }
    
    Value Value::lookupPath(Value &root, Value &temp, Value &now) {
        Value* current;
        switch (_pair->first._char) {
            case '^':
                current = &root;
                break;
            case '~':
                current = &temp;
                break;
            case '@':
                current = &now;
                break;
            default:
                return Value();
        }
        for (auto& key : _pair->second) {
            Value k = key.lookup(root, temp, now);
            current = &current->at(k);
        }
        return current;
    }
}