//
//  olfunction.cpp
//  ol-cpp
//
//  Created by 伍威 on 15/1/5.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include "olfunction.h"

using namespace std;
using namespace OL;
typedef ValuePtr (*func) (const vector<ValuePtr>& params, ValuePtr root, ValuePtr temp, ValuePtr now);
static map<string, func> table {
    {"+",[](const vector<ValuePtr>& params, ValuePtr root, ValuePtr temp, ValuePtr now)->ValuePtr {
        double ret = 0;
        for (auto& i: params) {
            ret += i->toNumber();
        }
        return ValuePtr(new Number(ret));
    }
    },
    {"-",[](const vector<ValuePtr>& params, ValuePtr root, ValuePtr temp, ValuePtr now)->ValuePtr {
        return ValuePtr(new Number(params[0]->toNumber() - params[1]->toNumber()));
    }
    },
};
namespace OL {
    
    ValuePtr calc(const string& name, const std::vector<ValuePtr>& params, ValuePtr root, ValuePtr temp, ValuePtr now) {
        auto f = table.find(name);
        if (f != table.end()) {
            return f->second(params, root, temp, now);
        }
        return nullptr;
    }

}
