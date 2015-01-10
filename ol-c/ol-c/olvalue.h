//
//  olvalue.h
//  ol-c
//
//  Created by 伍 威 on 15/1/9.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#ifndef __ol_c__olvalue__
#define __ol_c__olvalue__

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    OL_NULL, OL_STRING, OL_NUMBER, OL_BOOL, OL_PAIR, OL_ARRAY, OL_OBJECT, OL_PATH, OL_LIST, OL_NEGATIVE, OL_QUOTE
} OLType;

typedef struct OLArray OLArray;
typedef struct OLValue OLValue;
typedef struct OLPair OLPair;

struct OLArray {
    OLValue* buffer;
    size_t cap;
    size_t count;
};

OLArray* OLArrayCreate();
OLArray* OLArrayCleanup(OLArray* array);
OLValue* OLArrayAppend(OLArray* array);
OLValue OLArrayAtIndex(OLArray* array, size_t index);

struct OLValue {
    OLType type;
    union {
        double numberValue;
        char* stringValue;
        bool boolValue;
        OLArray* arrayValue;
        OLPair* pairValue;
    };
};
OLValue* OLValueCleanup(OLValue* value);
void OLValuePrint(OLValue value);

struct OLPair {
    OLValue key;
    OLValue value;
};
OLPair* OLPairCreate();
OLPair* OLPairCleanup(OLPair* pair);

extern OLValue OLNullValue;
#endif /* defined(__ol_c__olvalue__) */
