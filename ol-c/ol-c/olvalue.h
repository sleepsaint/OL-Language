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
#include "olarray.h"

typedef enum {
    OL_NULL, OL_STRING, OL_NUMBER, OL_BOOL, OL_PAIR, OL_ARRAY, OL_OBJECT, OL_PATH, OL_LIST, OL_NEGATIVE, OL_QUOTE
} OLType;

typedef struct OLValue OLValue;
typedef struct OLPair OLPair;


struct OLValue {
    OLType type;
    int ref;
    union {
        double numberValue;
        char* stringValue;
        bool boolValue;
        OLArray* arrayValue;
        OLPair* pairValue;
    };
};
struct OLPair {
    OLValue* key;
    OLValue* value;
};
OLPair* OLPairCreate();
OLPair* OLPairCleanup(OLPair* pair);

OLValue* OLValueCreate();
OLValue* OLValueCreateNumber(double number);
OLValue* OLValueCreateString(const char* begin, const char* end);
OLValue* OLValueCreateBool(bool value);
OLValue* OLValueCreateArray();
OLValue* OLValueCreateObject();
OLValue* OLValueCreatePair(OLPair* pair);

OLValue* OLValueRetain(OLValue* value);
OLValue* OLValueRelease(OLValue* value);
OLValue* OLValueAutoRelease(OLValue* value);

void OLValuePrint(OLValue* value);
void OLInit();

#endif /* defined(__ol_c__olvalue__) */
