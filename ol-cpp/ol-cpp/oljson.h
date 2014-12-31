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
        std::string _source;
        size_t _cursor;
        std::string _token;
    public:
        JSON(const std::string& source);
        Value* getValue();
        std::string getToken();
    private:
        bool match(const std::string& expected);
        void error(const std::string& e);
        Dictionary* getDictionary();
        Array* getArray();
        Number* getNumber();
        String* getString();
        Value* getBool();
        Value* getNull();
        bool getKeyValue(std::unique_ptr<Dictionary>& dict);
        bool getArrayValue(std::unique_ptr<Array>& array);
    };
}

#endif /* defined(__ol_cpp__oljson__) */
