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
    
    enum ValueType {
        
    };
    
    class Value {
    public:
        virtual std::string description() = 0;
    };
    
    typedef std::unique_ptr<Value> ValuePtr;
    
    class Number : public Value {
    public:
        Number(double value = 0);
        double _value;
        std::string description();
    };
    
    class String : public Value {
    public:
        std::string _value;
        std::string description();
    };
    
    class Array : public Value {
    public:
        std::vector<ValuePtr> _value;
        std::string description();

    };
    
    class Dictionary : public Value {
    public:
        std::map<std::string, ValuePtr> _value;
        std::string description();

    };
    
    class Null : public Value {
    public:
        std::string description();
    };
    
    class Path : public Value {
    public:
        std::string description();
    };
    
    class List : public Value {
    public:
        std::string description();
    };
    
    class Negative : public Value {
    public:
        std::string description();
    };
    
    class Quote : public Value {
    public:
        std::string description();
    };
    
    
}
#endif /* defined(__ol_cpp__olvalue__) */
