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
    
    class Value {
    public:
        Value() : _ref(1), _inAutoReleasePool(false){}
        virtual ~Value() {}
        virtual std::string description() { return "null"; }
        virtual Value* lookup(Value* root, Value* temp, Value* now) { return nullptr; }
        virtual void change(Value* root, Value* temp, Value* now, Value* to) {}
        virtual Value*& at(const std::string& key) { static Value* ptr; return ptr; }
        virtual Value*& operator[](const std::string& key) { static Value* ptr; return ptr; }
        virtual double toNumber() { return 0; }
        virtual int compare(const Value* v) { return 0; }
        virtual Value* filter(Value* func, Value* root, Value* temp) { return nullptr; }
        virtual operator bool() { return false; }
        virtual void sort(std::vector<Value*>& array, Value* root, Value* temp) {}
        virtual void toArray(std::vector<Value*>&) {}
        virtual bool some(Value* func, Value* root, Value* temp) { return false; }
        Value* retain() { if (this) ++_ref; return this; }
        Value* release() { if (this) { --_ref; if (!_ref) delete this; } return nullptr; }
        Value* autoRelease();
        static void doAutoRelease();
    private:
        int _ref;
        bool _inAutoReleasePool;
    };
    
    class String : public Value {
        std::string _value;
    public:
        String(const char* begin, const char* end) : _value(begin, end - begin) {}
        String(const std::string& s) : _value(s) {}
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
        std::vector<Value*> _value;
        ~Array();
        void append(Value* item) { _value.push_back(item); }
        std::string description() override;
        Value*& at(const std::string& key) { return _value.at(stoi(key)); }
        Value*& operator[](const std::string& key) { return _value[stoi(key)]; }
        Value* filter(Value* func, Value* root, Value* temp) override;
        operator bool() override { return _value.size() > 0; }
        void toArray(std::vector<Value*>& v) override { v = _value; }
        bool some(Value* func, Value* root, Value* temp) override;
    };
    
    class Object : public Value {
        std::map<std::string, Value*> _value;
    public:
        Object() {}
        Object(const std::map<std::string, Value*>& o) : _value(o) {}
        ~Object();
        Value*& at(const std::string& key) { return _value.at(key); }
        Value*& operator[](const std::string& key) { return _value[key]; }
        std::string description() override;
        Value* filter(Value* func, Value* root, Value* temp) override;
        operator bool() override { return _value.size() > 0; }
        void toArray(std::vector<Value*>&) override;
        bool some(Value* func, Value* root, Value* temp) override;
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
        int _root;
        std::vector<Value*> _keys;
    public:
        Path(char root) : _root(root) {}
        ~Path();
        void append(Value* key) { _keys.push_back(key); }
        std::string description() override;
        Value* lookup(Value* root, Value* temp, Value* now) override;
        void change(Value* root, Value* temp, Value* now, Value* to) override;
        void sort(std::vector<Value*>& array, Value* root, Value* temp) override;
    };
    
    class List : public Value {
        Value* _head;
        std::vector<Value*> _tail;
    public:
        List(Value* head) : _head(head) {}
        ~List();
        void append(Value* item) { _tail.push_back(item); }
        std::string description() override;
        Value* lookup(Value* root, Value* temp, Value* now) override;
        void sort(std::vector<Value*>& array, Value* root, Value* temp) override;
    };
    
    class Negative : public Value {
    public:
        Value* _value;
        Negative(Value* value) : _value(value) {}
        ~Negative() {_value->release();}
        std::string description() override { return "!" + _value->description(); }
        Value* lookup(Value* root, Value* temp, Value* now) override;
        void sort(std::vector<Value*>& array, Value* root, Value* temp) override;
    };
    
    class Quote : public Value {
        Value* _value;
    public:
        Quote(Value* value) : _value(value) {}
        ~Quote() {_value->release();}
        std::string description() override { return "#" + _value->description(); }
        Value* lookup(Value* root, Value* temp, Value* now) override {
            return _value;
        }
    };
    
    Value* autoLookup(Value* call, Value* root, Value* temp, Value* now);
}
#endif /* defined(__ol_cpp__olvalue__) */
