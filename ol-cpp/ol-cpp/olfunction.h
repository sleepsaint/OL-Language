//
//  olfunction.h
//  ol-cpp
//
//  Created by 伍威 on 15/1/5.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__olfunction__
#define __ol_cpp__olfunction__

#include "olvalue.h"

namespace OL {
    
    Value* calc(const std::string& name, const std::vector<Value*>& params, Value* root, Value* temp, Value* now);
}

#endif /* defined(__ol_cpp__olfunction__) */
