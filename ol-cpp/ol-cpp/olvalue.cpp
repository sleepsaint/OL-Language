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
    
    ValueIter Value::begin() const {
        return _ptr ? _ptr->begin() : ValueIter();
    }
    
    ValueIter Value::end() const {
        return _ptr ? _ptr->end() : ValueIter();
    }
        
    Value::Value(bool b) {
        _ptr = new Bool(b);
    }
    
    Value::Value(int n) {
        _ptr = new Number(n);
    }
    
    Value::Value(double n) {
        _ptr = new Number(n);
    }
    
    Value::Value(const char* str) {
        _ptr = new String(str);
    }
    
    Value::Value(const std::string& str) {
        _ptr = new String(str);
    }
    
    Value::~Value() {
        if (_ptr) {
            _ptr->release();
        }
    }
    
    Value::Value(const Value& v) {
        _ptr = v._ptr->retain();
    }
    
    Value::Value(const map<string, Value>& v) {
        _ptr = new Object(v);
    }

    Value::Value(const vector<Value>& v) {
        _ptr = new Array(v);
    }

    Value::Value(const pair<string, Value>& v) {
        _ptr = new Pair(v);
    }
    
    Value& Value::operator=(const Value &v) {
        if (_ptr != v._ptr) {
            if (_ptr) {
                _ptr->release();
            }
            _ptr = v._ptr;
            if (_ptr) {
                _ptr->retain();
            }
        }
        return *this;
    }
    
    Value& Value::operator=(ValueBase* v) {
        if (_ptr != v) {
            if (_ptr) {
                _ptr->release();
            }
            _ptr = v;
        }
        return *this;
    }
   
    bool Value::isNegative() const {
        return _ptr && typeid(*_ptr) == typeid(Negative);
    }
    
    bool Value::isArray() const {
        return _ptr && typeid(*_ptr) == typeid(Array);
    }
    
    bool Value::isObject() const {
        return _ptr && typeid(*_ptr) == typeid(Object);
    }
    
    bool Value::isNumber() const {
        return _ptr && typeid(*_ptr) == typeid(Number);
    }
    
    bool Value::isString() const {
        return _ptr && typeid(*_ptr) == typeid(String);
    }
    
    bool Value::isPair() const {
        return _ptr && typeid(*_ptr) == typeid(Pair);
    }
    
    std::string Value::description() const {
        return _ptr ? _ptr->description() : "null";
    }
    
    Value Value::lookup(const Value &root, const Value &temp, const Value &now) const {
        return _ptr ? _ptr->lookup(root, temp, now) : Value();
    }
    
    void Value::change(const Value &root, const Value &temp, const Value &now, const Value &to) const {
        if (_ptr) {
            _ptr->change(root, temp, now, to);
        }
    }
    
    void Value::remove(const Value &root, const Value &temp, const Value &now) const {
        if (_ptr) {
            _ptr->remove(root, temp, now);
        }
    }
        
    Value& Value::operator[](const std::string &key) const {
        static Value nullValue;
        return _ptr ? (*_ptr)[key] : nullValue;
    }

    Value& Value::operator[](const char* key) const {
        static Value nullValue;
        return _ptr ? (*_ptr)[string(key ? key : "")] : nullValue;
    }

    Value& Value::operator[](int key) const {
        static Value nullValue;
        return _ptr ? (*_ptr)[to_string(key)] : nullValue;
    }

    
    void Value::append(const Value &v) {
        if (_ptr && isArray()) {
            ((Array*)_ptr)->append(v);
        }
    }
    
    void Value::remove(const std::string& key) const {
        if (_ptr) {
            _ptr->remove(key);
        }
    }
    
    double Value::toNumber() const {
        return _ptr ? _ptr->toNumber() : 0;
    }
    
    std::vector<Value> Value::toArray() const {
        return _ptr ? _ptr->toArray() : std::vector<Value>();
    }
    
    int Value::compare(const Value &v) const {
        if (_ptr) {
            if (v.isNull()) {
                return 1;
            } else {
                return _ptr->compare(v);
            }
        } else {
            if (v.isNull()) {
                return 0;
            } else {
                return -1;
            }
        }
    }
    
    std::string Value::key() const {
        return isPair() ? ((Pair*)_ptr)->_value.first : "";
    }
    Value Value::value() const {
        static Value nullValue;
        return isPair() ? ((Pair*)_ptr)->_value.second : *this;
    }

    Value::operator bool() const {
        return _ptr && *_ptr;
    }
    
    Value Value::filter(const Value &func, const Value &root, const Value &temp) const {
        return _ptr ? _ptr->filter(func, root, temp) : Value();
    }
    
    void Value::sort(std::vector<Value> &array, const Value &root, const Value &temp) const {
        if (_ptr) {
            _ptr->sort(array, root, temp);
        }
    }
    
    bool Value::some(const Value &func, const Value &root, const Value &temp) const {
        return _ptr && _ptr->some(func, root, temp);
    }
    
    bool Value::operator <(const Value &v) const {
        return compare(v) < 0;
    }
    
    bool Value::operator >(const OL::Value &v) const {
        return compare(v) > 0;
    }
    
    bool Value::operator ==(const OL::Value &v) const {
        return compare(v) == 0;
    }
    
    double operator +(const Value& a, const Value& b) {
        return a.toNumber() + b.toNumber();
    }
    
    double operator +(double a, const Value& b) {
        return a + b.toNumber();
    }
    
    double operator +(const Value& a, double b) {
        return a.toNumber() + b;
    }

    double operator *(const Value& a, const Value& b) {
        return a.toNumber() * b.toNumber();
    }
    
    double operator *(double a, const Value& b) {
        return a * b.toNumber();
    }
    
    double operator *(const Value& a, double b) {
        return a.toNumber() * b;
    }

    double operator -(const Value& a, const Value& b) {
        return a.toNumber() - b.toNumber();
    }
    
    double operator /(const Value& a, const Value& b) {
        return a.toNumber() / b.toNumber();
    }
   
}