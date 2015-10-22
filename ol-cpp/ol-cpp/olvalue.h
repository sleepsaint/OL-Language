//
//  olvalue.h
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__olvalue__
#define __ol_cpp__olvalue__

#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace OL {
    
    class ValueBase;
    class ValueIter;
    
    class Value {
    public:
        Value() : _ptr(nullptr) {}
        Value(ValueBase* v) : _ptr(v) {}
        Value(bool b);
        Value(double n);
        Value(int n);
        Value(const char* str);
        Value(const std::string& str);
        Value(const Value& v);
        Value(const std::map<std::string, Value>& v);
        Value(const std::vector<Value>& v);
        ~Value();
        std::string description() const;
        Value lookup (const Value& root, const Value& temp, const Value& now) const;
        void change(const Value& root, const Value& temp, const Value& now, const Value& to) const;
        void remove(const Value& root, const Value& temp, const Value& now) const;
        void append(const Value& v);
        void remove(const std::string& key) const;
        std::vector<Value> toArray() const;
        int compare(const Value& v) const;
        Value filter(const Value& func, const Value& root, const Value& temp) const;
        void sort(std::vector<Value>& array, const Value& root, const Value& temp) const;
        bool some(const Value& func, const Value& root, const Value& temp) const;
        bool isNumber() const;
        bool isString() const;
        bool isNegative() const;
        bool isArray() const;
        bool isObject() const;
        bool isPair() const;
        bool isNull() const { return !_ptr; }
        
        double toNumber() const;
        std::string toString() const { return description(); };
        
        Value& operator [](const std::string& key) const;
        Value& operator [](const char* key) const;
        Value& operator [](int key) const;

        Value& operator =(const Value& v);
        Value& operator =(ValueBase* v);
        
        bool operator >(const Value& v) const;
        bool operator <(const Value& v) const;
        bool operator ==(const Value& v) const;
 
        operator bool() const;
        
        
        friend double operator +(const Value& a, const Value& b);
        friend double operator +(double a, const Value& b);
        friend double operator +(const Value& a, double b);
        friend double operator *(const Value& a, const Value& b);
        friend double operator *(double a, const Value& b);
        friend double operator *(const Value& a, double b);

        friend double operator -(const Value& a, const Value& b);
        friend double operator /(const Value& a, const Value& b);

        ValueBase* ptr() const { return _ptr; }
        
        ValueIter begin() const;
        ValueIter end() const;
        
        std::string key() const;
        Value& value() const;

    private:
        ValueBase* _ptr;
    };
    class ValueIter {
    public:
        enum IterType { UNSUPPORT, ARRAY, OBJECT };
        ValueIter() : _type(UNSUPPORT) {}
        ValueIter(const std::vector<Value>::iterator& iter) : _array(iter), _type(ARRAY) {}
        ValueIter(const std::map<std::string, Value>::iterator& iter) : _object(iter), _type(OBJECT) {}
        bool operator!= (const ValueIter& other) const
        {
            switch (_type) {
                case ARRAY:
                    return _array != other._array;
                case OBJECT:
                    return _object != other._object;
                default:
                    return false;
            }
        }
        
        Value& operator* () const {
            static Value nullValue;
            switch (_type) {
                case ARRAY:
                    return *_array;
                case OBJECT:
                    return _object->second;
                default:
                    return nullValue;
            }
        }
        
        const ValueIter& operator++ ()
        {
            switch (_type) {
                case ARRAY:
                    ++_array;
                    break;
                case OBJECT:
                    ++_object;
                    break;
                default:
                    break;
            }
            return *this;
        }
    private:
        std::vector<Value>::iterator _array;
        std::map<std::string, Value>::iterator _object;
        IterType _type;
    };
    

    typedef std::vector<OL::Value> ArrayValue;
    typedef std::map<std::string, OL::Value> ObjectValue;
    
}
#endif /* defined(__ol_cpp__olvalue__) */
