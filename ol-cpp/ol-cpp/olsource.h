//
//  olsource.h
//  ol-cpp
//
//  Created by 伍威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__olsource__
#define __ol_cpp__olsource__

#include "olbuffer.h"
#include "olvalue.h"

namespace OL {
    
    class Source {
        const char* _source;
        const char* _end;
        const char* _cursor;
        int _token;
        enum {
            STRING_TOKEN = 100000,
            NUMBER_TOKEN
        };
        Buffer _tokenString;
        double _tokenNumber;
        bool _tokenBool;
        std::string _errorLog;
    public:
        static Value parse(const char* source, size_t length);
    private:
        Source(const char* source, size_t length);
        void nextToken();
        void unescape();
        bool match(int expected) {
            if (_token == expected) {
                nextToken();
                return true;
            } else {
                return false;
            }
        }
        void error(const std::string& e);
        bool getNumber(Value& value);
        bool getString(Value& value);
        bool getPath(Value& value);
        bool getKey(std::vector<Value>& v);
        bool getFragment(Value& value);
        bool getList(Value& value);
        bool getNegative(Value& value);
        bool getQuote(Value& value);
        bool getValue(Value& value) {
            return getString(value) || getNumber(value) || getPath(value) || getList(value) || getNegative(value) || getQuote(value);
        }
    };
}

#endif /* defined(__ol_cpp__olsource__) */
