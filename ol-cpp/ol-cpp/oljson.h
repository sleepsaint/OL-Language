//
//  oljson.h
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__oljson__
#define __ol_cpp__oljson__

#include <string>
#include "olvalue.h"

namespace OL {

    class JSON {
        char* _source;
        char* _end;
        char* _cursor;
        int _token;
        enum {
            STRING_TOKEN = 100000,
            NUMBER_TOKEN,
            BOOL_TOKEN,
            NULL_TOKEN
        };
        char* _tokenString;
        char* _tokenStringEnd;
        double _tokenNumber;
        bool _tokenBool;
    public:
        static Value parse(char* source, size_t length) {
            JSON json(source, length);
            Value value;
            json.getValue(value);
            return value;
        }
        static Value* parse2(char* source, size_t length) {
            JSON json(source, length);
            Value* value = new Value;
            json.getValue(*value);
            return value;
        }
    private:
        JSON(char* source, size_t length);
        void nextToken();
        void unescape();
        bool getValue(Value& value) {
            return getString(value) || getNumber(value) || getObject(value) || getArray(value) || getBool(value) || getNull(value);
        }
        bool match(int expected) {
            if (_token == expected) {
                nextToken();
                return true;
            } else {
                return false;
            }
        }
        bool getObject(Value& value);
        bool getArray(Value& value);
        bool getNumber(Value& value);
        bool getString(Value& value);
        bool getBool(Value& value);
        bool getNull(Value& value);
        bool getPair(Value& object);
        bool getElement(Value& array);
    };
}

#endif /* defined(__ol_cpp__oljson__) */