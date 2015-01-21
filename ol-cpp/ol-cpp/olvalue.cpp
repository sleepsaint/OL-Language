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
    
    static vector<Value*> autoReleasePool;
    
    Value* Value::autoRelease() {
        autoReleasePool.push_back(this);
        return this;
    }
    
    void Value::doAutoRelease() {
        for (auto& i : autoReleasePool) {
            i->release();
        }
        autoReleasePool.clear();
    }
    
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
    
    Value* autoLookup(Value* call, Value* root, Value* temp, Value* now) {
        Value* ret = call;
        while (typeid(*ret) == typeid(String)) {
            Value* source = Source::parse(ret->description());
            if (source) {
                ret = source->lookup(root, temp, now);
            }
        }
        return ret;
    }
    
    Value* Path::lookup(Value* root, Value* temp, Value* now) {
        Value* current;
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
            current = autoLookup(current, root, temp, now);
            if (key && current) {
                current = (*current)[key->description()];
            } else {
                return nullptr;
            }
        }
        return current;
    }
    
    Value* List::lookup(Value* root, Value* temp, Value* now) {
        Value* name = _head->lookup(root, temp, now);
        if (name) {
            vector<Value*> params;
            params.resize(_tail.size());
            transform(_tail.begin(), _tail.end(), params.begin(), [=](Value* v)->Value*{return v->lookup(root, temp, now);});
            return calc(name->description(), params, root, temp, now);
        }
        return nullptr;
    }
    
    Value* Negative::lookup(Value* root, Value* temp, Value* now) {
        Value* p = _value->lookup(root, temp, now);
        return new Bool(!(p && *p));
    }
    template<typename T> int compare(const T& a, const T& b) {
        if (a > b) {
            return 1;
        } else if (a < b) {
            return -1;
        } else {
            return 0;
        }
    }
    int String::compare(const Value* v) {
        const String* s = dynamic_cast<const String*>(v);
        if (s) {
            return ::OL::compare(_value, s->_value);
        }
        const Number* n = dynamic_cast<const Number*>(v);
        if (n) {
            return ::OL::compare(_value, s->_value);
        }
        return 0;
    }
    
    int Number::compare(const Value* v) {
        const Number* s = dynamic_cast<const Number*>(v);
        if (s) {
            return ::OL::compare(_value, s->_value);
        }
        return 0;
    }
    
    int Bool::compare(const Value* v) {
        const Bool* b = dynamic_cast<const Bool*>(v);
        if (b) {
            return ::OL::compare(_value, b->_value);
        }
        return 0;
    }
    
    Value* Array::filter(Value* func, Value* root, Value* temp) {
        Array* ret = new Array;
        ret->_value.resize(_value.size());
        auto it = copy_if(_value.begin(), _value.end(), ret->_value.begin(),
                          [=](Value*& v)->bool{
                              Value* p = func->lookup(root, temp, v);
                              return p && *p;
                          });
        ret->_value.resize(distance(ret->_value.begin(), it));
        return ret;
    }

    Value* Object::filter(Value* func, Value* root, Value* temp) {
        Object* ret = new Object;
        for (auto i : _value) {
            Value* p = func->lookup(root, temp, i.second);
            if (p && *p) {
                ret->_value[i.first] = i.second;
            }
        }
        return ret;
    }
    
    inline int compare(Value* a, Value* b, Value* call, Value* root, Value* temp) {
        if (a && b) {
            Negative* negative = dynamic_cast<Negative*>(call);
            if (negative) {
                return -compare(a, b, negative->_value, root, temp);
            } else {
                Value* left = call->lookup(root, temp, a);
                Value* right = call->lookup(root, temp, b);
                return left->compare(right);
            }

        } else {
            return ::OL::compare(a, b);
        }
    }
    
    void Path::sort(std::vector<Value*>& array, Value* root, Value* temp) {
        ::sort(array.begin(), array.end(), [=](Value* a, Value* b)->bool{
            int ret = ::OL::compare(a, b, this, root, temp);
            return ret < 0;
        });
    }

    void List::sort(std::vector<Value*>& array, Value* root, Value* temp) {
        ::sort(array.begin(), array.end(), [=](Value* a, Value* b)->bool{
            int ret = ::OL::compare(a, b, _head, root, temp);
            for (const auto& i : _tail) {
                if (ret != 0) {
                    return ret < 0;
                } else {
                    ret = ::OL::compare(a, b, i, root, temp);
                }
            }
            return ret < 0;
        });
    }
    
    void Negative::sort(std::vector<Value*>& array, Value* root, Value* temp) {
        ::sort(array.begin(), array.end(), [=](Value* a, Value* b)->bool{
            int ret = ::OL::compare(a, b, _value, root, temp);
            return ret > 0;
        });
    }
    
    void Object::toArray(std::vector<Value*>& v) {
        v.resize(_value.size());
        transform(_value.begin(), _value.end(), v.begin(), [](const pair<string, Value*>& i)->Value*{return i.second;});
    }
    
    bool Array::some(Value* func, Value* root, Value* temp) {
        for (const auto& i : _value) {
            if (func->lookup(root, temp, i)) {
                return true;
            }
        }
        return false;
    }
    
    bool Object::some(Value* func, Value* root, Value* temp) {
        for (const auto& i : _value) {
            if (func->lookup(root, temp, i.second)) {
                return true;
            }
        }
        return false;
    }

    Array::~Array() {
        for (auto& i : _value) {
            i->release();
        }
    }
    
    Object::~Object() {
        for (auto& i : _value) {
            i.second->release();
        }
    }
    
    Path::~Path() {
        for (auto& i : _keys) {
            i->release();
        }
    }
    
    List::~List() {
        for (auto& i : _tail) {
            i->release();
        }
    }
    
}