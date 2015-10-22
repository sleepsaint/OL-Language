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
    
    class ValueBase;
    class Quote;
    class Path;
    class List;
    
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
        static Value parse(const std::string& source);
        static bool debug;
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
        Quote* getNumber();
        Quote* getString();
        Path* getPath(int root);
        ValueBase* getKey();
        Path* getFragment();
        List* getList();
        ValueBase* getValue();
    };
}

#endif /* defined(__ol_cpp__olsource__) */
