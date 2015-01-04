//
//  oljson.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include "oljson.h"
#include <iostream>
using namespace std;

namespace OL {
    
    JSON::JSON(const char* source, size_t length)
    : _source(source), _end(source + length), _cursor(source), _token(0) {
        nextToken();
    }
    
    void JSON::nextToken() {
        const char* start;
        if (_cursor < _end) {
            auto c = *_cursor;
            switch (c) {
                case '{':
                case '}':
                case '[':
                case ']':
                case ',':
                case ':':
                    _token = c;
                    ++_cursor;
                    break;
                case '"':
                    _token = STRING_TOKEN;
                    ++_cursor;
                    unescape();
                    break;
                case 'n':
                    _token = NULL_TOKEN;
                    _cursor += 4;
                    break;
                case 't':
                    _token = BOOL_TOKEN;
                    _cursor += 4;
                    _tokenBool = true;
                    break;
                case 'f':
                    _token = BOOL_TOKEN;
                    _cursor += 5;
                    _tokenBool = false;
                    break;
                case '\n':
                case ' ':
                    ++_cursor;
                    nextToken();
                    break;
                default:
                    _token = NUMBER_TOKEN;
                    start = _cursor;
                    while (_cursor < _end) {
                        switch (*_cursor) {
                            case '{':
                            case '}':
                            case '[':
                            case ']':
                            case ',':
                                goto PP;
                            default:
                                ++_cursor;
                        }
                    }
                PP: _tokenNumber = atof(start);
            }
        } else {
            _token = 0;
        }
    }
    
    bool JSON::getNumber(Value& value) {
        if (match(NUMBER_TOKEN)) {
            value._type = Value::Number;
            value._number = _tokenNumber;
            return true;
        }
        return false;
    }
    
    bool JSON::getString(Value& value) {
        if (match(STRING_TOKEN)) {
            value._type = Value::String;
            value._string = new string(_tokenString.begin(), _tokenString.end());
            return true;
        }
        return false;
    }
    
    bool JSON::getBool(Value& value) {
        if (match(BOOL_TOKEN)) {
            value._type = Value::Bool;
            value._bool = _tokenBool;
            return true;
        }
        return false;
    }
    
    bool JSON::getNull(Value& value) {
        if (match(NULL_TOKEN)) {
            return true;
        }
        return false;
    }
    
    bool JSON::getObject(Value& value) {
        if (match('{')) {
            value._type = Value::Object;
            value._object = new map<string, Value>;
            if (getPair(value)) {
                while (match(',')) {
                    if (!getPair(value)) {
                        return false;
                    }
                }
            }
            if (match('}')) {
                return true;
            }
        }
        return false;
    }
    
    bool JSON::getArray(Value& value) {
        if (match('[')) {
            value._type = Value::Array;
            value._array = new vector<Value>;
            if (getElement(value)) {
                while (match(',')) {
                    if (!getElement(value)) {
                        return false;
                    }
                }
            }
            if (match(']')) {
                return true;
            }
        }
        return false;
    }
    
    bool JSON::getPair(Value &object) {
        string key(_tokenString.begin(), _tokenString.end());
        if (match(STRING_TOKEN) && match(':')) {
            Value value;
            if (getValue(value)) {
                object._object->insert(pair<string, Value>(key, move(value)));
                return true;
            }
        }
        return false;
    }
    
    bool JSON::getElement(Value& array) {
        Value value;
        if (getValue(value)) {
            array._array->push_back(move(value));
            return true;
        }
        return false;
    }
    
    inline int char_to_num(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        } else {
            return 0;
        }
    }
    
    void JSON::unescape() {
        _tokenString.reset();
        while (_cursor < _end) {
            auto c = *_cursor++;
            if (c == '\\') {
                auto d = *_cursor++;
                switch (d) {
                    case '"':
                        _tokenString.append('"');
                        break;
                    case '\\':
                        _tokenString.append('\\');
                        break;
                    case '/':
                        _tokenString.append('/');
                        break;
                    case 'b':
                        _tokenString.append('\b');
                        break;
                    case 'f':
                        _tokenString.append('\f');
                        break;
                    case 'n':
                        _tokenString.append('\n');
                        break;
                    case 'r':
                        _tokenString.append('\r');
                        break;
                    case 't':
                        _tokenString.append('\t');
                        break;
                    case 'u':
                    {
                        int hex = (char_to_num(_cursor[0]) << 12) | (char_to_num(_cursor[1]) << 8) | (char_to_num(_cursor[2]) << 4) | char_to_num(_cursor[3]);
                        _cursor += 4;
                        if (hex < 0x80) {
                            _tokenString.append(hex);
                        } else if (hex < 0x800) {
                            _tokenString.append(0xc0 | (hex >> 6));
                            _tokenString.append(0x80 | (hex & 0x3f));
                        } else if (hex < 0x10000) {
                            _tokenString.append(0xe0 | (hex >> 12));
                            _tokenString.append(0x80 | ((hex >> 6) & 0x3f));
                            _tokenString.append(0x80 | (hex & 0x3f));
                        }
                        
                    }
                        
                        break;
                    default:
                        _tokenString.append(d);
                }
            } else {
                if (c == '"') {
                    return;
                } else {
                    _tokenString.append(c);
                }
            }
        }
    }
    
}