//
//  olvaluebase.hpp
//  ol-cpp
//
//  Created by 伍 威 on 15/10/21.
//  Copyright © 2015年 sleepsaint. All rights reserved.
//

#ifndef olvaluebase_hpp
#define olvaluebase_hpp

#include <vector>
#include <map>
#include <string>
#include "olvalue.h"

namespace OL {

    class ValueBase {
    public:
        ValueBase() : _ref(1) {}
        virtual ~ValueBase() {}
        virtual std::string description() { return "null"; }
        virtual Value lookup(const Value& root, const Value& temp, const Value& now) const { return Value(); }
        virtual void change(const Value& root, const Value& temp, const Value& now, const Value& to) const {}
        virtual void remove(const Value& root, const Value& temp, const Value& now) {}
        virtual Value& operator[](const std::string& key) { static Value v; return v; }
        virtual void remove(const std::string& key) {}
        virtual double toNumber() { return 0; }
        virtual int compare(const Value& v) { return 0; }
        virtual Value filter(const Value& func, const Value& root, const Value& temp) const { return Value(); }
        virtual operator bool() { return false; }
        virtual void sort(std::vector<Value>& array, const Value& root, const Value& temp) const {}
        virtual std::vector<Value> toArray() { return std::vector<Value>(); }
        virtual bool some(const Value& func, const Value& root, const Value& temp) const { return false; }
        virtual ValueIter begin() { return ValueIter(); }
        virtual ValueIter end() { return ValueIter(); }

        ValueBase* retain() { auto a = this; if (a) ++_ref; return this; }
        ValueBase* release() { auto a = this; if (a) { --_ref; if (!_ref) delete this; } return nullptr; }
        
    private:
        int _ref;
    };
    
    class String : public ValueBase {
        std::string _value;
    public:
        String(const char* begin, const char* end) : _value(begin, end - begin) {}
        String(const std::string& s) : _value(s) {}
        std::string description() override { return _value; }
        double toNumber() override { return std::stod(_value); }
        int compare(const Value& v) override;
        operator bool() override { return _value.length() > 0; }
    };
    
    class Number : public ValueBase {
        double _value;
    public:
        Number(double number) : _value(number) {}
        std::string description() override;
        double toNumber() override { return _value; }
        int compare(const Value& v) override;
        operator bool() override { return _value != 0; }
    };
    
    class Array  : public ValueBase {
    public:
        Array() {};
        Array(const std::vector<Value>& v) : _value(v) {}
        std::vector<Value> _value;
        void append(const Value& item) { _value.push_back(item); }
        std::string description() override;
        Value& operator[](const std::string& key) override { return _value[stoi(key)]; }
        void remove(const std::string& key) override { int i = stoi(key); _value.erase(_value.begin() + i); };
        Value filter(const Value& func, const Value& root, const Value& temp) const override;
        operator bool() override { return _value.size() > 0; }
        std::vector<Value> toArray() override { return _value; }
        bool some(const Value &root, const Value &temp, const Value &now) const override;
        ValueIter begin() override { return ValueIter(_value.begin()); }
        ValueIter end() override { return ValueIter(_value.end()); }

    };
    
    class Pair : public ValueBase {
    public:
        std::pair<std::string, Value> _value;
        Pair(){}
        Pair(const std::pair<std::string, Value>& v) : _value(v) {}
        std::string description() override { return _value.first + ":" + _value.second.description(); };
        operator bool() override { return _value.second; }
    };
    
    class Object : public ValueBase {
        std::map<std::string, Value> _value;
    public:
        Object() {}
        Object(const std::map<std::string, Value>& v) : _value(v) {}
        Value& operator[](const std::string& key) override { return _value[key]; }
        void remove(const std::string& key) override {_value.erase(key);};
        std::string description() override;
        Value filter(const Value& func, const Value& root, const Value& temp) const override;
        operator bool() override { return _value.size() > 0; }
        std::vector<Value> toArray() override;
        bool some(const Value &root, const Value &temp, const Value &now) const override;
        ValueIter begin() override { return ValueIter(_value.begin()); }
        ValueIter end() override { return ValueIter(_value.end()); }

    };
    
    class Bool : public ValueBase {
        bool _value;
    public:
        Bool(bool b) : _value(b) {}
        std::string description() override;
        double toNumber() override { return _value ? 1 : 0; }
        int compare(const Value& v) override;
        operator bool() override { return _value; }
        
    };
    
    class Path : public ValueBase {
        int _root;
        std::vector<Value> _keys;
    public:
        Path(char root) : _root(root) {}
        void append(const Value& key) { _keys.push_back(key); }
        std::string description() override;
        Value lookup(const Value& root, const Value& temp, const Value& now) const override;
        void change(const Value& root, const Value& temp, const Value& now, const Value& to) const override;
        void remove(const Value& root, const Value& temp, const Value& now) override;
        void sort(std::vector<Value>& array, const Value& root, const Value& temp) const override;
    };
    
    class List : public ValueBase {
        Value _head;
        std::vector<Value> _tail;
    public:
        List(const Value& head) : _head(head) {}
        void append(const Value& item) { _tail.push_back(item); }
        std::string description() override;
        Value lookup(const Value& root, const Value& temp, const Value& now) const override;
        void sort(std::vector<Value>& array, const Value& root, const Value& temp) const override;
    };
    
    class Negative : public ValueBase {
    public:
        Value _value;
        Negative(const Value& value) : _value(value) {}
        std::string description() override { return "!" + _value.description(); }
        Value lookup(const Value& root, const Value& temp, const Value& now) const override;
        void sort(std::vector<Value>& array, const Value& root, const Value& temp) const override;
    };
    
    class Quote : public ValueBase {
        Value _value;
    public:
        Quote(const Value& value) : _value(value) {}
        std::string description() override { return "#" + _value.description(); }
        Value lookup(const Value& root, const Value& temp, const Value& now) const override {
            return _value;
        }
    };
    
}
#endif /* olvaluebase_hpp */
