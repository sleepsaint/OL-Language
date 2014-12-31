//
//  oljson.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include "oljson.h"

using namespace std;

namespace OL {
    
    JSON::JSON(const string& source) : _source(source), _cursor(0) {
        _token = getToken();
    }
    
    string JSON::getToken() {
        auto end = _source.length();
        auto start = end;
        for (auto i = _cursor; i < end; ++i) {
            auto c = _source[i];
            switch (c) {
                case '{':
                case '}':
                case '[':
                case ']':
                case ',':
                case ':':
                case '"':
                    if (start == end) {
                        ++_cursor;
                        return string(1, c);
                    } else {
                        _cursor = i;
                        return _source.substr(start, _cursor - start);
                    }
                case '\n':
                    ++_cursor;
                default:
                    if (start == end) {
                        start = _cursor;
                    }
            }
        }
        _cursor = end;
        return start == end ? "" : _source.substr(start);
    }
    
    bool JSON::match(const std::string &expected) {
        if (_token == expected) {
            _token = getToken();
            return true;
        } else {
            return false;
        }
    }
    
    bool JSON::getKeyValue(std::unique_ptr<Dictionary> &dict) {
        unique_ptr<String> key(getString());
        if (match(":")) {
            dict->_value[key->_value] = ValuePtr(getValue());
            return true;
        } else {
            return false;
        }
    }
    
    Dictionary* JSON::getDictionary() {
        if (match("{")) {
            unique_ptr<Dictionary> dict(new Dictionary);
            if (getKeyValue(dict)) {
                while (match(",")) {
                    if (!getKeyValue(dict)) {
                        return nullptr;
                    }
                }
            }
            if (match("}")) {
                return dict.release();
            }
        }
        return nullptr;
    }
    
    bool JSON::getArrayValue(std::unique_ptr<Array> &array) {
        ValuePtr value(getValue());
        if (value) {
            array->_value.push_back(std::move(value));
            return true;
        } else {
            return false;
        }
    }
    
    Array* JSON::getArray() {
        if (match("[")) {
            unique_ptr<Array> array(new Array);
            if (getArrayValue(array)) {
                while (match(",")) {
                    if (!getArrayValue(array)) {
                        return nullptr;
                    }
                }
            }
            if (match("]")) {
                return array.release();
            }
        }
        return nullptr;
    }
    
    String* JSON::getString() {
        if (match("\"")) {
            String* s = new String;
            s->_value = _token;
            _token = getToken();
            if (match("\"")) {
                return s;
            } else {
                delete s;
            }
        }
        return nullptr;
    }
    
    Number* JSON::getNumber() {
        if (_token.length() > 0) {
            auto c = _token[0];
            if (c == '-' || (c >= '0' && c <= '9')){
                Number* n = new Number;
                n->_value = stod(_token);
                _token = getToken();
                return n;
            }
        }
        return nullptr;
    }
    
    Value* JSON::getBool() {
        if (_token == "true") {
            _token = getToken();
            return new Number(1);
        } else if (_token == "false") {
            _token = getToken();
            return new Number(0);
        }
        return nullptr;
    }
    
    Value* JSON::getNull() {
        if (_token == "null") {
            _token = getToken();
            return new Null;
        }
        return nullptr;
    }
    
    Value* JSON::getValue() {
        Value* ret = nullptr;
        if ((ret = getString())) {
            return ret;
        } else if ((ret = getNumber())) {
            return ret;
        } else if ((ret = getDictionary())) {
            return ret;
        } else if ((ret = getArray())) {
            return ret;
        } else if ((ret = getBool())) {
            return ret;
        } else if ((ret = getNull())) {
            return ret;
        } else {
            return nullptr;
        }
    }
    
}