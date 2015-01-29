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

struct OLArray {
    size_t* buffer;
    size_t cap;
    size_t count;
};

size_t OLArrayCreate();
size_t OLArrayCleanup(size_t array);
void OLArrayAppend(size_t array, size_t value);
size_t OLArrayAtIndex(size_t array, size_t index);

struct OLValue {
    OLType type;
    int ref;
    union {
        double numberValue;
        char* stringValue;
        bool boolValue;
        size_t pointerValue;
    };
};
struct OLPair {
    size_t key;
    size_t value;
};
size_t OLPairCreate();
size_t OLPairCleanup(size_t pair);

size_t OLValueCreate();
size_t OLValueCreateNumber(double number);
size_t OLValueCreateString(const char* begin, const char* end);
size_t OLValueCreateBool(bool value);
size_t OLValueCreateArray();
size_t OLValueCreateObject();
size_t OLValueCreatePair(size_t pair);

size_t OLValueRetain(size_t value);
size_t OLValueRelease(size_t value);
size_t OLValueAutoRelease(size_t value);

void OLValuePrint(size_t value);
void OLInit();

extern struct OLArray* OLArray;
extern struct OLPair* OLPair;
extern struct OLValue* OLValue;

#endif /* defined(__ol_c__olvalue__) */
