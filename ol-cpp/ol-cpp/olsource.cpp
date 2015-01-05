//
//  olsource.cpp
//  ol-cpp
//
//  Created by 伍威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "olsource.h"

using namespace std;

namespace OL {
    
    enum ScanStatus {
        None, String, Number
    };
    
    Source::Source(const char* source, size_t length)
    : _source(source), _end(source + length), _cursor(source), _token(0) {
        nextToken();
    }
    
    void Source::nextToken() {
        if (_cursor < _end) {
            auto c = *_cursor;
            const char* start;
            switch (c) {
                case '^':
                case '@':
                case '~':
                case '!':
                case '{':
                case '}':
                case '(':
                case ')':
                case ',':
                case '#':
                case '.':
                    _token = c;
                    ++_cursor;
                    break;
                case '\n':
                case ' ':
                    ++_cursor;
                    nextToken();
                    break;
                case '$':
                    _token = NUMBER_TOKEN;
                    ++_cursor;
                    start = _cursor;
                    while (_cursor < _end) {
                        switch (*_cursor) {
                            case '^':
                            case '@':
                            case '~':
                            case '!':
                            case '{':
                            case '}':
                            case '(':
                            case ')':
                            case ',':
                            case '#':
                            case '\n':
                            case ' ':
                            case '$':
                                goto PP;
                            default:
                                ++_cursor;
                        }
                    }
                PP: _tokenNumber = atof(start);
                    break;
                default:
                    _token = STRING_TOKEN;
                    unescape();
            }
        } else {
            _token = 0;
        }
    }
    void Source::unescape() {
        _tokenString.reset();
        while (_cursor < _end) {
            auto c = *_cursor;
            char d;
            switch (c) {
                case '^':
                case '@':
                case '~':
                case '!':
                case '{':
                case '}':
                case '(':
                case ')':
                case ',':
                case '#':
                case '.':
                case '$':
                    return;
                case '\\':
                    ++_cursor;
                    d = *_cursor;
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
                        default:
                            _tokenString.append(d);
                    }
                    ++_cursor;
                    break;
                default:
                    _tokenString.append(c);
                    ++_cursor;
                    break;
            }
        }
    }
    
    void Source::error(const std::string &e) {
        if (_errorLog.empty()) {
            ostringstream s;
            s <<  e <<  " at " << (_cursor - _source);
            _errorLog = s.str();
        }
    }
    
    bool Source::getNumber(Value& value) {
        if (match(NUMBER_TOKEN)) {
            value._type = Value::Number;
            value._number = _tokenNumber;
            return true;
        }
        return false;
    }
    
    bool Source::getString(Value& value) {
        if (match(STRING_TOKEN)) {
            value._type = Value::String;
            value._string = new string(_tokenString.begin(), _tokenString.end());
            return true;
        }
        return false;
    }
    
    bool Source::getPath(Value &value) {
        int token = _token;
        if (match('^') || match('~') || match('@')) {
            value._type = Value::Path;
            value._pair = new pair<Value, vector<Value>>;
            value._pair->first._type = Value::Char;
            value._pair->first._char = token;
            while (match('.')) {
                if (!getKey(value._pair->second)) {
                    error("can not match a key");
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    
    bool Source::getKey(std::vector<Value> &v) {
        Value value;
        if (getString(value) || getFragment(value) || getList(value)) {
            v.push_back(move(value));
            return true;
        } else {
            return false;
        }
    }
    
    bool Source::getFragment(Value &value) {
        if (match('{')) {
            if (getPath(value)) {
                if (match('}')) {
                    return true;
                } else {
                    error("can not match }");
                }
            } else {
                error("in {} must a path");
            }
        }
        return false;
    }
    
    bool Source::getList(Value &value) {
        if (match('(')) {
            value._type = Value::List;
            value._pair = new pair<Value, vector<Value>>;
            if (getValue(value._pair->first)) {
                while (match(',')) {
                    Value item;
                    if (getValue(item)) {
                        value._pair->second.push_back(move(item));
                    } else {
                        error("tail can not match a value");
                        return false;
                    }
                }
                if (match(')')) {
                    return true;
                } else {
                    error("can not match )");
                }
            }
        }
        return false;
    }
    
    bool Source::getNegative(Value &value) {
        if (match('!')) {
            value._type = Value::Negative;
            value._value = new Value;
            if (getValue(*value._value)) {
                return true;
            } else {
                error("can not match value for !");
            }
        }
        return false;
    }
    
    bool Source::getQuote(Value &value) {
        if (match('#')) {
            value._type = Value::Quote;
            value._value = new Value;
            if (getValue(*value._value)) {
                return true;
            } else {
                error("can not match value for #");
            }
        }
        return false;
    }
    
    Value Source::parse(const char *source, size_t length) {
        Value value;
        Source s(source, length);
        if (s.getValue(value)) {
            return value;
        } else {
            cout << "parse error: " << string(source, length) << endl;
            cout << s._errorLog << endl;
            return Value();
        }
    }
}