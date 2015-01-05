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
        enum ValueType {
            Null, Char, Number, String, Bool, Array, Object, Path, List, Negative, Quote, Pointer
        };
        Value();
        ~Value();
        Value(const Value& value) = delete;
        Value(Value&& value);
        Value(bool b);
        Value(Value* pointer);
        std::string description();
        operator bool();
        Value& operator [](const Value& key) { return this->at(key); }
        Value& at(const Value& key);
        bool isNull() { return _type == Null; }
        Value lookup(Value& root, Value& temp, Value& now);
    private:
        friend class JSON;
        friend class Source;
        ValueType _type;
        std::string descriptionNull();
        std::string descriptionChar();
        std::string descriptionNumber();
        std::string descriptionString();
        std::string descriptionBool();
        std::string descriptionArray();
        std::string descriptionObject();
        std::string descriptionPath();
        std::string descriptionList();
        std::string descriptionNegative();
        std::string descriptionQuote();
        Value lookupPath(Value& root, Value& temp, Value& now);
        union {
            double _number;
            bool _bool;
            char _char;
            std::string* _string;
            std::vector<Value>* _array;
            std::map<std::string, Value>* _object;
            std::pair<Value, std::vector<Value>>* _pair;
            Value* _value;
        };
    };
}
#endif /* defined(__ol_cpp__olvalue__) */
