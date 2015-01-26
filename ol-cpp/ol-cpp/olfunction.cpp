//
//  olfunction.cpp
//  ol-cpp
//
//  Created by 伍威 on 15/1/5.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <cstdlib>
#include <climits>
#include <cmath>
#include "olfunction.h"

using namespace std;
using namespace OL;

typedef Value* (*FUNC) (const std::vector<Value*>& params, Value* root, Value* temp, Value* now);

static map<string, FUNC> table {
    {"+", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        double ret = 0;
        for (auto& i: params) {
            ret += i->toNumber();
        }
        return new Number(ret);
    }
    },
    {"-", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Number(params[0]->toNumber() - params[1]->toNumber());
    }
    },
    {"*", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        if (params.size() == 0) return new Number(0);
            double ret = 1;
            for (auto& i: params) {
                ret *= i->toNumber();
            }
        return new Number(ret);
    }
    },
    {"/", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Number(params[0]->toNumber() / params[1]->toNumber());
    }
    },
    {">", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Bool(params[0]->compare(params[1]) > 0);
    }
    },
    {">=", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Bool(params[0]->compare(params[1]) >= 0);
    }
    },
    {"=", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Bool(params[0]->compare(params[1]) == 0);
    }
    },
    {"<", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Bool(params[0]->compare(params[1]) < 0);
    }
    },
    {"<=", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Bool(params[0]->compare(params[1]) <= 0);
    }
    },
    {"!=", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Bool(params[0]->compare(params[1]) != 0);
    }
    },
    {"not", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        return new Bool(!(params[0] && *params[0]));
    }
    },
    {"or", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        for (const auto& i : params) {
            if (i && *i) {
                return new Bool(true);
            }
        }
        return new Bool(false);
    }
    },
    {"and", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        for (const auto& i : params) {
            if (!(i && *i)) {
                return new Bool(false);
            }
        }
        return new Bool(params.size() > 0);
    }
    },
    
    {"filter", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        Value* container = autoLookup(params[0], root, temp, now);
        Value* func = autoLookup(params[1], root, temp, now);
        if (container && func) {
            return container->filter(func, root, temp);
        }
        return nullptr;
    }},
    {"sort", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        Value* container = autoLookup(params[0], root, temp, now);
        Value* func = autoLookup(params[1], root, temp, now);
        if (container && func) {
            auto array = new Array;
            container->toArray(array->_value);
            func->sort(array->_value, root, temp);
            return array;
        }
        return nullptr;
    }},
    {"some", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        Value* container = autoLookup(params[0], root, temp, now);
        Value* func = autoLookup(params[1], root, temp, now);
        if (container && func) {
            return new Bool(container->some(func, root, temp));
        }
        return nullptr;
    }},
    {"random", [](const std::vector<Value*>& params, Value* root, Value* temp, Value* now)->Value* {
        double ret = (double)arc4random() / (double)UINT_MAX;
        switch (params.size()) {
            case 0:
                break;
            case 1:
                ret = floor(ret * params[0]->toNumber());
                break;
            default:
                ret = params[0]->toNumber() + floor((params[1]->toNumber() - params[0]->toNumber()) * ret);
        }
        return new Number(ret);
    }}
    
};

namespace OL {
    
    Value* calc(const string& name, const std::vector<Value*>& params, Value* root, Value* temp, Value* now) {
        auto f = table.find(name);
        if (f != table.end()) {
            return f->second(params, root, temp, now)->autoRelease();
        }
        return nullptr;
    }
    
}
