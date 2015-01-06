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
        ValuePtr p = _value->lookup(_value, root, temp, now);
        return ValuePtr(new Bool(!(p && *p)));
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
    int String::compare(OL::Value *v) {
        String* s = dynamic_cast<String*>(v);
        if (s) {
            return ::OL::compare(_value, s->_value);
        }
        Number* n = dynamic_cast<Number*>(v);
        if (n) {
            return ::OL::compare(_value, s->_value);
        }
        return 0;
    }
    
    int Number::compare(OL::Value *v) {
        Number* s = dynamic_cast<Number*>(v);
        if (s) {
            return ::OL::compare(_value, s->_value);
        }
        return 0;
    }
    
    int Bool::compare(OL::Value *v) {
        Bool* b = dynamic_cast<Bool*>(v);
        if (b) {
            return ::OL::compare(_value, b->_value);
        }
        return 0;
    }
    
    ValuePtr Array::filter(ValuePtr func, ValuePtr root, ValuePtr temp) {
        Array* ret = new Array;
        ret->_value.resize(_value.size());
        auto it = copy_if(_value.begin(), _value.end(), ret->_value.begin(),
                          [=](ValuePtr& v)->bool{
                              ValuePtr p = func->lookup(func, root, temp, v);
                              return p ? *p : false;
                          });
        ret->_value.resize(distance(ret->_value.begin(), it));
        return ValuePtr(ret);
    }

    ValuePtr Object::filter(ValuePtr func, ValuePtr root, ValuePtr temp) {
        Object* ret = new Object;
        for (auto i : _value) {
            ValuePtr p = func->lookup(func, root, temp, i.second);
            if (p && *p) {
                ret->_value[i.first] = i.second;
            }
        }
        return ValuePtr(ret);
    }
    
    inline int compare(const ValuePtr& a, const ValuePtr& b, const ValuePtr& call, ValuePtr root, ValuePtr temp) {
        if (a && b) {
            Negative* negative = dynamic_cast<Negative*>(call.get());
            if (negative) {
                return -compare(a, b, negative->_value, root, temp);
            } else {
                ValuePtr left = call->lookup(call, root, temp, a);
                ValuePtr right = call->lookup(call, root, temp, b);
                return left->compare(right.get());
            }

        } else {
            return ::OL::compare(a, b);
        }
    }
    
    void Path::sort(const ValuePtr& call, std::vector<ValuePtr>& array, ValuePtr root, ValuePtr temp) {
        ::sort(array.begin(), array.end(), [=](const ValuePtr& a, const ValuePtr& b)->bool{
            int ret = ::OL::compare(a, b, call, root, temp);
            return ret < 0;
        });
    }

    void List::sort(const ValuePtr& call, std::vector<ValuePtr>& array, ValuePtr root, ValuePtr temp) {
        ::sort(array.begin(), array.end(), [=](const ValuePtr& a, const ValuePtr& b)->bool{
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
    
    void Negative::sort(const ValuePtr &call, std::vector<ValuePtr> &array, ValuePtr root, ValuePtr temp) {
        ::sort(array.begin(), array.end(), [=](const ValuePtr& a, const ValuePtr& b)->bool{
            int ret = ::OL::compare(a, b, _value, root, temp);
            return ret > 0;
        });
    }
    
    void Object::toArray(std::vector<ValuePtr>& v) {
        v.resize(_value.size());
        transform(_value.begin(), _value.end(), v.begin(), [](const pair<string, ValuePtr>& i)->ValuePtr{return i.second;});
    }
    
    bool Array::some(ValuePtr func, ValuePtr root, ValuePtr temp) {
        for (const auto& i : _value) {
            if (func->lookup(func, root, temp, i)) {
                return true;
            }
        }
        return false;
    }
    
    bool Object::some(ValuePtr func, ValuePtr root, ValuePtr temp) {
        for (const auto& i : _value) {
            if (func->lookup(func, root, temp, i.second)) {
                return true;
            }
        }
        return false;
    }

}