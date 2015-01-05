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
        virtual ValuePtr lookup(ValuePtr root, ValuePtr temp, ValuePtr now) { return ValuePtr(this); }
        virtual ValuePtr& at(const std::string& key) { static ValuePtr ptr; return ptr; }
        virtual ValuePtr& operator[](const std::string& key) { static ValuePtr ptr; return ptr; }

    };
    
    class String : public Value {
        std::string _value;
    public:
        String(const char* begin, const char* end) : _value(begin, end - begin) {}
        std::string description() override { return _value; }
    };
    
    class Number : public Value {
        double _value;
    public:
        Number(double number) : _value(number) {}
        std::string description() override;
    };
    
    class Array  : public Value {
        std::vector<ValuePtr> _value;
    public:
        void append(Value* item) { _value.push_back(ValuePtr(item)); }
        std::string description() override;
        ValuePtr& at(const std::string& key) { return _value.at(stoi(key)); }
        ValuePtr& operator[](const std::string& key) { return _value[stoi(key)]; }
    };
    
    class Object : public Value {
        std::map<std::string, ValuePtr> _value;
    public:
        ValuePtr& at(const std::string& key) { return _value.at(key); }
        ValuePtr& operator[](const std::string& key) { return _value[key]; }
        std::string description() override;
    };
    
    class Bool : public Value {
        bool _value;
    public:
        Bool(bool b) : _value(b) {}
        std::string description() override;
    };
    
    class Path : public Value {
        char _root;
        std::vector<ValuePtr> _keys;
    public:
        Path(char root) : _root(root) {}
        void append(Value* key) { _keys.push_back(ValuePtr(key)); }
        std::string description() override;
        ValuePtr lookup(ValuePtr root, ValuePtr temp, ValuePtr now) override;
    };
    
    class List : public Value {
        ValuePtr _head;
        std::vector<ValuePtr> _tail;
    public:
        List(Value* head) : _head(head) {}
        void append(Value* item) { _tail.push_back(ValuePtr(item)); }
        std::string description() override;
        ValuePtr lookup(ValuePtr root, ValuePtr temp, ValuePtr now) override;
    };
    
    class Negative : public Value {
        ValuePtr _value;
    public:
        Negative(Value* value) : _value(value) {}
        std::string description() override { return "!" + _value->description(); }
        ValuePtr lookup(ValuePtr root, ValuePtr temp, ValuePtr now) override;
    };
    
    class Quote : public Value {
        ValuePtr _value;
    public:
        Quote(Value* value) : _value(value) {}
        std::string description() override { return "#" + _value->description(); }
        ValuePtr lookup(ValuePtr root, ValuePtr temp, ValuePtr now) override {
            return _value;
        }
    };
    
    
}
#endif /* defined(__ol_cpp__olvalue__) */
