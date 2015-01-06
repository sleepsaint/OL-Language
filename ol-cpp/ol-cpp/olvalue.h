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

namespace OL {
    class Value;
    typedef std::shared_ptr<Value> ValuePtr;

    class Value {
    public:
        virtual ~Value() {}
        virtual std::string description() { return "null"; }
        virtual ValuePtr lookup(const ValuePtr& call, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now) { return call; }
        virtual ValuePtr& at(const std::string& key) { static ValuePtr ptr; return ptr; }
        virtual ValuePtr& operator[](const std::string& key) { static ValuePtr ptr; return ptr; }
        virtual double toNumber() { return 0; }
        virtual int compare(const Value* v) { return 0; }
        virtual ValuePtr filter(const ValuePtr& func, const ValuePtr& root, const ValuePtr& temp) { return nullptr; }
        virtual operator bool() { return false; }
        virtual void sort(const ValuePtr& call, std::vector<ValuePtr>& array, const ValuePtr& root, const ValuePtr& temp) {}
        virtual void toArray(std::vector<ValuePtr>&) {}
        virtual bool some(const ValuePtr& func, const ValuePtr& root, const ValuePtr& temp) { return false; }
    };
    
    class String : public Value {
        std::string _value;
    public:
        String(const char* begin, const char* end) : _value(begin, end - begin) {}
        std::string description() override { return _value; }
        double toNumber() override { return std::stod(_value); }
        int compare(const Value* v) override;
        operator bool() override { return _value.length() > 0; }
    };
    
    class Number : public Value {
        double _value;
    public:
        Number(double number) : _value(number) {}
        std::string description() override;
        double toNumber() { return _value; }
        int compare(const Value* v) override;
        operator bool() override { return _value != 0; }
    };
    
    class Array  : public Value {
    public:
        std::vector<ValuePtr> _value;
        Array() {}
        void append(Value* item) { _value.push_back(ValuePtr(item)); }
        std::string description() override;
        ValuePtr& at(const std::string& key) { return _value.at(stoi(key)); }
        ValuePtr& operator[](const std::string& key) { return _value[stoi(key)]; }
        ValuePtr filter(const ValuePtr& func, const ValuePtr& root, const ValuePtr& temp) override;
        operator bool() override { return _value.size() > 0; }
        void toArray(std::vector<ValuePtr>& v) override { v = _value; }
        bool some(const ValuePtr& func, const ValuePtr& root, const ValuePtr& temp) override;
    };
    
    class Object : public Value {
        std::map<std::string, ValuePtr> _value;
    public:
        ValuePtr& at(const std::string& key) { return _value.at(key); }
        ValuePtr& operator[](const std::string& key) { return _value[key]; }
        std::string description() override;
        ValuePtr filter(const ValuePtr& func, const ValuePtr& root, const ValuePtr& temp) override;
        operator bool() override { return _value.size() > 0; }
        void toArray(std::vector<ValuePtr>&) override;
        bool some(const ValuePtr& func, const ValuePtr& root, const ValuePtr& temp) override;
    };
    
    class Bool : public Value {
        bool _value;
    public:
        Bool(bool b) : _value(b) {}
        std::string description() override;
        double toNumber() override { return _value ? 1 : 0; }
        int compare(const Value* v) override;
        operator bool() override { return _value; }

    };
    
    class Path : public Value {
        char _root;
        std::vector<ValuePtr> _keys;
    public:
        Path(char root) : _root(root) {}
        void append(Value* key) { _keys.push_back(ValuePtr(key)); }
        std::string description() override;
        ValuePtr lookup(const ValuePtr& call, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now) override;
        void sort(const ValuePtr& call, std::vector<ValuePtr>& array, const ValuePtr& root, const ValuePtr& temp) override;
    };
    
    class List : public Value {
        ValuePtr _head;
        std::vector<ValuePtr> _tail;
    public:
        List(Value* head) : _head(head) {}
        void append(Value* item) { _tail.push_back(ValuePtr(item)); }
        std::string description() override;
        ValuePtr lookup(const ValuePtr& call, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now) override;
        void sort(const ValuePtr& call, std::vector<ValuePtr>& array, const ValuePtr& root, const ValuePtr& temp) override;
    };
    
    class Negative : public Value {
    public:
        ValuePtr _value;
        Negative(Value* value) : _value(value) {}
        std::string description() override { return "!" + _value->description(); }
        ValuePtr lookup(const ValuePtr& call, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now) override;
        void sort(const ValuePtr& call, std::vector<ValuePtr>& array, const ValuePtr& root, const ValuePtr& temp) override;
    };
    
    class Quote : public Value {
        ValuePtr _value;
    public:
        Quote(Value* value) : _value(value) {}
        std::string description() override { return "#" + _value->description(); }
        ValuePtr lookup(const ValuePtr& call, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now) override {
            return _value;
        }
    };
    
    ValuePtr autoLookup(ValuePtr call, ValuePtr root, ValuePtr temp, ValuePtr now);
    
}
#endif /* defined(__ol_cpp__olvalue__) */
