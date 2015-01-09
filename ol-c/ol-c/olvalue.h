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

struct OLPair {
    char* key;
    OLValue value;
};

OLArray* createOLArray();
inline OLArray* cleanupOLArray(OLArray* array);
inline void appendOLArray(OLArray* array, OLValue value);
inline OLValue valueAtIndex(OLArray* array, size_t index);
OLValue* cleanupOLValue(OLValue* value);
extern OLValue nullValue;
#endif /* defined(__ol_c__olvalue__) */
