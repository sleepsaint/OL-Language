//
//  oljson.h
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__oljson__
#define __ol_cpp__oljson__

#include "olbuffer.h"
#include "olvalue.h"
#include "olvaluebase.h"

namespace OL {
    class Object;
    class Array;
    class JSON {
        const char* _source;
        const char* _end;
        const char* _cursor;
        int _token;
        enum {
            STRING_TOKEN = 100000,
            NUMBER_TOKEN,
            BOOL_TOKEN,
            NULL_TOKEN
        };
        Buffer _tokenString;
        double _tokenNumber;
        bool _tokenBool;
    public:
        static Value parse(const char* source, size_t length) {
            return JSON(source, length).getValue();
        }
        static Value parse(const std::string& source) {
            return JSON(source.c_str(), source.length()).getValue();
        }
    private:
        JSON(const char* source, size_t length);
        void nextToken();
        void unescape();
        ValueBase* getValue();
        bool match(int expected) {
            if (_token == expected) {
                nextToken();
                return true;
            } else {
                return false;
            }
        }
        Object* getObject();
        Array* getArray();
        bool getPair(Object* object);
        bool getElement(Array* array);
    };
}

#endif /* defined(__ol_cpp__oljson__) */
