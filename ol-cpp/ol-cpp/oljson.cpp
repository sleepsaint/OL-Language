//
//  oljson.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include <cstdlib>
#include "oljson.h"

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
    
    Value* JSON::getNumber() {
        if (match(NUMBER_TOKEN)) {
            return new Number(_tokenNumber);
        }
        return nullptr;
    }
    
    Value* JSON::getString() {
        if (match(STRING_TOKEN)) {
            return new String(_tokenString.begin(), _tokenString.end());
        }
        return nullptr;
    }
    
    Value* JSON::getBool() {
        if (match(BOOL_TOKEN)) {
            return new Bool(_tokenBool);
        }
        return nullptr;
    }
    
    Value* JSON::getNull() {
        if (match(NULL_TOKEN)) {
            return new Value;
        }
        return nullptr;
    }
    
    Value* JSON::getObject() {
        if (match('{')) {
            Object* object = new Object;
            if (getPair(object)) {
                while (match(',')) {
                    if (!getPair(object)) {
                        delete object;
                        return nullptr;
                    }
                }
            }
            if (match('}')) {
                return object;
            } else {
                delete object;
            }
        }
        return nullptr;
    }
    
    Value* JSON::getArray() {
        if (match('[')) {
            Array* array = new Array;
            if (getElement(array)) {
                while (match(',')) {
                    if (!getElement(array)) {
                        delete array;
                        return nullptr;
                    }
                }
            }
            if (match(']')) {
                return array;
            } else {
                delete array;
            }
        }
        return nullptr;
    }
    
    bool JSON::getPair(Object* object) {
        string key(_tokenString.begin(), _tokenString.end());
        if (match(STRING_TOKEN) && match(':')) {
            Value* value = getValue();
            if (value) {
                (*object)[key] = ValuePtr(value);
                return true;
            }
        }
        return false;
    }
    
    bool JSON::getElement(Array* array) {
        Value* value = getValue();
        if (value) {
            array->append(value);
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