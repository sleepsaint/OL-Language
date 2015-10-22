//
//  olvaluebase.cpp
//  ol-cpp
//
//  Created by 伍 威 on 15/10/21.
//  Copyright © 2015年 sleepsaint. All rights reserved.
//

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
#include "olvaluebase.h"
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
            s << i->description();
            ++i;
            while (i != _value.end()) {
                s << "," << i->description();
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
            s << i->first << ":" << i->second.description();
            ++i;
            while (i != _value.end()) {
                s << "," << i->first << ":" << i->second.description();
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
            s << "[" << i.description() << "]";
        }
        return s.str();
    }
    
    string List::description() {
        ostringstream s;
        s << _head.description() << "(";
        auto i = _tail.begin();
        if (i != _tail.end()) {
            s << i->description();
            ++i;
            while (i != _tail.end()) {
                s << "," << i->description();
                ++i;
            }
        }
        s << ")";
        return s.str();
    }
    
    string Bool::description() {
        return _value ? "true" : "false";
    }
    
    Value Path::lookup(const Value& root, const Value& temp, const Value& now) const {
        Value current;
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
                return current;
        }
        for (auto& k : _keys) {
            auto key = k.lookup(root, temp, now);
            if (key && current) {
                current = current[key.description()];
            } else {
                return Value();
            }
        }
        return current;
    }
    void Path::change(const Value& root, const Value& temp, const Value& now, const Value& to) const {
        Value current;
        Value previous;
        Value key;
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
                return;
        }
        for (auto& k : _keys) {
            key = k.lookup(root, temp, now);
            if (key && current) {
                previous = current;
                current = current[key.description()];
            } else {
                return;
            }
        }
        previous[key.description()] = to;
    }
    void Path::remove(const Value& root, const Value& temp, const Value& now) {
        Value current;
        Value previous;
        Value key;
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
                return;
        }
        for (auto& k : _keys) {
            key = k.lookup(root, temp, now);
            if (key && current) {
                previous = current;
                current = current[key.description()];
            } else {
                return;
            }
        }
        previous.remove(key.description());
    }
    
    Value List::lookup(const Value& root, const Value& temp, const Value& now) const {
        Value name = _head.lookup(root, temp, now);
        if (name) {
            vector<Value> params;
            for (auto& i : _tail) {
                params.push_back(i.lookup(root, temp, now));
            }
            return calc(name.description(), params, root, temp, now);
        }
        return Value();
    }
    
    Value Negative::lookup(const Value& root, const Value& temp, const Value& now) const {
        return !_value.lookup(root, temp, now);
    }

    int String::compare(const Value& v) {
        if (v.isString()) {
            return _value.compare(v.toString());
        } else {
            return toNumber() - v.toNumber();
        }
    }
    
    int Number::compare(const Value& v) {
        return toNumber() - v.toNumber();
    }
    
    int Bool::compare(const Value& v) {
        if (*this) {
            if (v) {
                return 0;
            } else {
                return 1;
            }
        } else {
            if (v) {
                return -1;
            } else {
                return 0;
            }
        }
    }
    
    Value Array::filter(const Value& func, const Value& root, const Value& temp) const {
        Array* ret = new Array;
        for (auto& i : _value) {
            if (func.lookup(root, temp, i)) {
                ret->append(i);
            }
        }
        return ret;
    }
    
    Value Object::filter(const Value& func, const Value& root, const Value& temp) const {
        Object* ret = new Object;
        for (auto& i : _value) {
            const Value& v = i.second;
            if (func.lookup(root, temp, v)) {
                (*ret)[i.first] = v;
            }
        }
        return ret;
    }
    
    inline int compare(const Value& a, const Value& b, const ValueBase* call, const Value& root, const Value& temp) {
        if (typeid(*call) == typeid(Negative)) {
            return -compare(a, b, ((Negative*)call)->_value.ptr(), root, temp);
        } else {
            Value left = call->lookup(root, temp, a);
            Value right = call->lookup(root, temp, b);
            return left.compare(right);
        }
    }
    
    void Path::sort(std::vector<Value>& array, const Value& root, const Value& temp) const {
        ::sort(array.begin(), array.end(), [=](const Value& a, const Value& b)->bool{
            int ret = ::OL::compare(a, b, this, root, temp);
            return ret < 0;
        });
    }
    
    void List::sort(std::vector<Value>& array, const Value& root, const Value& temp) const {
        ::sort(array.begin(), array.end(), [=](const Value& a, const Value& b)->bool{
            int ret = ::OL::compare(a, b, _head.ptr(), root, temp);
            for (const auto& i : _tail) {
                if (ret != 0) {
                    return ret < 0;
                } else {
                    ret = ::OL::compare(a, b, i.ptr(), root, temp);
                }
            }
            return ret < 0;
        });
    }
    
    void Negative::sort(std::vector<Value>& array, const Value& root, const Value& temp) const {
        ::sort(array.begin(), array.end(), [=](const Value& a, const Value& b)->bool{
            int ret = ::OL::compare(a, b, _value.ptr(), root, temp);
            return ret > 0;
        });
    }
    
    std::vector<Value> Object::toArray() {
        std::vector<Value> ret;
        for (auto& i : _value) {
            ret.push_back(i.second);
        }
        return ret;
    }
    
    bool Array::some(const Value& func, const Value& root, const Value& temp) const {
        for (const auto& i : _value) {
            if (func.lookup(root, temp, i)) {
                return true;
            }
        }
        return false;
    }
    
    bool Object::some(const Value& func, const Value& root, const Value& temp) const {
        for (const auto& i : _value) {
            if (func.lookup(root, temp, i.second)) {
                return true;
            }
        }
        return false;
    }
    
}