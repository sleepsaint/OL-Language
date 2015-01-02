//
//  olsource.cpp
//  ol-cpp
//
//  Created by 伍威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include "olsource.h"

using namespace std;

namespace OL {
    
    enum ScanStatus {
        None, String, Number
    };
    
    Source::Source(const string& source) : _source(source), _cursor(0) {
        _token = getToken();
    }
    
    string Source::getToken() {
        auto start = _cursor;
        auto status = None;
        auto end = _source.length();
        for (auto i = _cursor; i < end; ++i) {
            auto c = _source[i];
            switch (c) {
                case '~':
                case '!':
                case '@':
                case '^':
                case '(':
                case ')':
                case '{':
                case '}':
                case ',':
                case '#':
                    if (status == None) {
                        ++_cursor;
                        return string(1, c);
                    } else {
                        _cursor = i;
                        return _source.substr(start, _cursor - start);
                    }
                case '.':
                    if (status == None) {
                        ++_cursor;
                        return ".";
                    } else if (status == String) {
                        _cursor = i;
                        return _source.substr(start, _cursor - start);
                    }
                    break;
                case ' ':
                    ++_cursor;
                    break;
                case '$':
                    if (status == None) {
                        start = _cursor;
                        status = Number;
                    }
                    break;
                default:
                    if (status == None) {
                        start = _cursor;
                        status = String;
                    }
                    break;
            }
        }
        _cursor = end;
        return (start == end || status == None) ? "" : _source.substr(start);
    }
    
    bool Source::match(const std::string &expected) {
        if (_token == expected) {
            _token = getToken();
            return true;
        } else {
            return false;
        }
    }
    
    
    
    
}