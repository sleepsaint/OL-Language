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
    
    bool Source::debug = false;
    
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
                        case '\\':
                            _tokenString.append('\\');
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
        if (debug && _errorLog.empty()) {
            ostringstream s;
            s <<  e <<  " at " << (_cursor - _source);
            _errorLog = s.str();
        }
    }
    
    Quote* Source::getNumber() {
        if (match(NUMBER_TOKEN)) {
            return new Quote(new class Number(_tokenNumber));
        }
        return nullptr;
    }
    
    Quote* Source::getString() {
        if (match(STRING_TOKEN)) {
            return new Quote(new class String(_tokenString.begin(), _tokenString.end()));
        }
        return nullptr;
    }
    
    Path* Source::getPath(int root) {
        Path* path = new Path(root);
        while (match('.')) {
            Value* key = getKey();
            if (key) {
                path->append(key);
            } else {
                error("can not match a key");
                path->release();
                return nullptr;
            }
        }
        return path;
    }
    
    Path* Source::getFragment() {
        auto token = _token;
        Path* path;
        nextToken();
        switch (token) {
            case '^':
            case '@':
            case '~':
                path = getPath(token);
                if (path) {
                    if (match('}')) {
                        return path;
                    } else {
                        error("can not match }");
                        path->release();
                    }
                } else {
                    error("in {} must a path");
                }
            default:
                break;
        }
        return nullptr;
    }
    
    List* Source::getList() {
        Value* head = getValue();
        if (head) {
            List* list = new List(head);
            while (match(',')) {
                Value* item = getValue();
                if (item) {
                    list->append(item);
                } else {
                    error("tail can not match a value");
                    break;
                }
            }
            if (match(')')) {
                return list;
            } else {
                error("can not match )");
            }
            list->release();
        }
        return nullptr;
    }
    
    Value* Source::parse(const char *source, size_t length) {
        Source s = Source(source, length);
        auto ret = s.getValue();
        if (debug && !ret) {
            cout << string(source, length) << endl;
            cout << s._errorLog << endl;
        }
        return ret->autoRelease();
    }
    
    Value* Source::parse(const std::string &source) {
        return parse(source.c_str(), source.length());
    }
    
    Value* Source::getValue() {
        auto token = _token;
        Value* value;
        nextToken();
        switch (token) {
            case STRING_TOKEN:
                return new Quote(new class String(_tokenString.begin(), _tokenString.end()));
            case NUMBER_TOKEN:
                return new Quote(new class Number(_tokenNumber));
            case '^':
            case '@':
            case '~':
                return getPath(token);
            case '{':
                return getFragment();
            case '!':
                value = getValue();
                if (value) {
                    return new Negative(value);
                } else {
                    error("can not match value for !");
                    return nullptr;
                }
            case '#':
                value = getValue();
                if (value) {
                    return new Quote(value);
                } else {
                    error("can not match value for #");
                    return nullptr;
                }
            case '(':
                return getList();
            default:
                return nullptr;
        }
    }
    
    Value* Source::getKey() {
        auto token = _token;
        nextToken();
        switch (token) {
            case STRING_TOKEN:
                return new Quote(new class String(_tokenString.begin(), _tokenString.end()));
            case '{':
                return getFragment();
            case '(':
                return getList();
            default:
                return nullptr;
        }
    }
    
}