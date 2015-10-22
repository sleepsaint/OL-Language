//
//  olfunction.cpp
//  ol-cpp
//
//  Created by 伍威 on 15/1/5.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cmath>
#include "olfunction.h"
#include "olvaluebase.h"

using namespace std;
using namespace OL;

typedef Value (*FUNC) (const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now);

static map<string, FUNC> table {
    {"+", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        double ret = 0;
        for (const auto& i: params) {
            ret = ret + i;
        }
        return ret;
    }
    },
    {"-", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] - params[1];
    }
    },
    {"*", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        if (params.size() == 0) return 0;
        double ret = 1;
        for (auto& i: params) {
            ret = ret * i;
        }
        return ret;
    }
    },
    {"/", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] / params[1];
    }
    },
    {">", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] > params[1];
    }
    },
    {">=", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] >= params[1];
    }
    },
    {"=", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] == params[1];
    }
    },
    {"<", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] < params[1];
    }
    },
    {"<=", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] <= params[1];
    }
    },
    {"!=", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] != params[1];
    }
    },
    {"not", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return !params[0];
    }
    },
    {"or", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        for (const auto& i : params) {
            if (i) {
                return true;
            }
        }
        return false;
    }
    },
    {"and", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        for (const auto& i : params) {
            if (!i) {
                return false;
            }
        }
        return params.size() > 0;
    }
    },
    {"if",  [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] ? params[1] : params[2];
    }
    },
    {"default",  [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0] ? params[0] : params[1];
    }
    },

    {"filter", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0].filter(params[1], root, temp);
    }},
    {"sort", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        std::vector<Value> ret = params[0].toArray();
        params[1].sort(ret, root, temp);
        return ret;
    }},
    {"some", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        return params[0].some(params[1], root, temp);
    }},
    {"random", [](const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now)->Value {
        double ret = (double)arc4random() / (double)UINT_MAX;
        switch (params.size()) {
            case 0:
                break;
            case 1:
                ret = floor(ret * params[0].toNumber());
                break;
            default:
                ret = params[0].toNumber() + floor((params[1].toNumber() - params[0].toNumber()) * ret);
        }
        return ret;
    }}
    
};

namespace OL {
    
    Value calc(const string& name, const std::vector<Value>& params, const Value& root, const Value& temp, const Value& now) {
        auto f = table.find(name);
        if (f != table.end()) {
            return f->second(params, root, temp, now);
        }
        return Value();
    }
    
}
