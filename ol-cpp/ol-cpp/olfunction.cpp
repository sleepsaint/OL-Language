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

typedef ValuePtr (*FUNC) (const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now);

static map<string, FUNC> table {
    {"+", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        double ret = 0;
        for (auto& i: params) {
            ret += i->toNumber();
        }
        return ValuePtr(new Number(ret));
    }
    },
    {"-", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Number(params[0]->toNumber() - params[1]->toNumber()));
    }
    },
    {"*", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        if (params.size() == 0) return ValuePtr(new Number(0));
            double ret = 1;
            for (auto& i: params) {
                ret *= i->toNumber();
            }
        return ValuePtr(new Number(ret));
    }
    },
    {"/", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Number(params[0]->toNumber() / params[1]->toNumber()));
    }
    },
    {">", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Bool(params[0]->compare(params[1].get()) > 0));
    }
    },
    {">=", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Bool(params[0]->compare(params[1].get()) >= 0));
    }
    },
    {"=", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Bool(params[0]->compare(params[1].get()) == 0));
    }
    },
    {"<", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Bool(params[0]->compare(params[1].get()) < 0));
    }
    },
    {"<=", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Bool(params[0]->compare(params[1].get()) <= 0));
    }
    },
    {"!=", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Bool(params[0]->compare(params[1].get()) != 0));
    }
    },
    {"not", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        return ValuePtr(new Bool(!(params[0] && *params[0])));
    }
    },
    {"or", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        for (const auto& i : params) {
            if (i && *i) {
                return ValuePtr(new Bool(true));
            }
        }
        return ValuePtr(new Bool(false));
    }
    },
    {"and", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        for (const auto& i : params) {
            if (!(i && *i)) {
                return ValuePtr(new Bool(false));
            }
        }
        return ValuePtr(new Bool(params.size() > 0));
    }
    },
    
    {"filter", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        ValuePtr container = autoLookup(params[0], root, temp, now);
        ValuePtr func = autoLookup(params[1], root, temp, now);
        if (container && func) {
            return container->filter(func, root, temp);
        }
        return nullptr;
    }},
    {"sort", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        ValuePtr container = autoLookup(params[0], root, temp, now);
        ValuePtr func = autoLookup(params[1], root, temp, now);
        if (container && func) {
            auto array = new Array;
            container->toArray(array->_value);
            func->sort(func, array->_value, root, temp);
            return ValuePtr(array);
        }
        return nullptr;
    }},
    {"some", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
        ValuePtr container = autoLookup(params[0], root, temp, now);
        ValuePtr func = autoLookup(params[1], root, temp, now);
        if (container && func) {
            return ValuePtr(new Bool(container->some(func, root, temp)));
        }
        return nullptr;
    }},
    {"random", [](const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now)->ValuePtr {
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
        return ValuePtr(new Number(ret));
    }}
    
};

namespace OL {
    
    ValuePtr calc(const string& name, const std::vector<ValuePtr>& params, const ValuePtr& root, const ValuePtr& temp, const ValuePtr& now) {
        auto f = table.find(name);
        if (f != table.end()) {
            return f->second(params, root, temp, now);
        }
        return nullptr;
    }
    
}
