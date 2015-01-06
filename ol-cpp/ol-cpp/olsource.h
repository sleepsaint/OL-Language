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
    
    class Value;
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
        static ValuePtr parse(const char* source, size_t length);
        static ValuePtr parse(const std::string& source);
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
        Number* getNumber();
        String* getString();
        Path* getPath();
        Value* getKey(){
            Value* ret;
            if ((ret = getString())) {
                return ret;
            } else if ((ret = getFragment())) {
                return ret;
            } else if ((ret = getList())) {
                return ret;
            } else {
                return nullptr;
            }
        }
        Path* getFragment();
        List* getList();
        Negative* getNegative();
        Quote* getQuote();
        Value* getValue() {
            Value* ret;
            if ((ret = getString())) {
                return ret;
            } else if ((ret = getNumber())) {
                return ret;
            } else if ((ret = getPath())) {
                return ret;
            } else if ((ret = getList())) {
                return ret;
            } else if ((ret = getNegative())) {
                return ret;
            } else if ((ret = getQuote())) {
                return ret;
            } else {
                return nullptr;
            }
        }
    };
}

#endif /* defined(__ol_cpp__olsource__) */
