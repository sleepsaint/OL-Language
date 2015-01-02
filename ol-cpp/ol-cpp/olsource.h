//
//  olsource.h
//  ol-cpp
//
//  Created by 伍威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__olsource__
#define __ol_cpp__olsource__

#include <string>
#include "olvalue.h"

namespace OL {
    
    class Source {
        std::string _source;
        size_t _cursor;
        std::string _token;
        std::string _errorLog;
    public:
        Source(const std::string& source);
        std::string getToken();
    private:
        bool match(const std::string& expected);
        void error(const std::string& e);
        Value* getLiteral();
        Value* getPath();
        Value* getString();
        Value* getKey();
        Value* getFragment();
        Value* getList();
        Value* getNegative();
        Value* getQuote();
        Value* getValue();
    };
}

#endif /* defined(__ol_cpp__olsource__) */
