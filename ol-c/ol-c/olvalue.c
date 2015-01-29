//
//  olvalue.c
//  ol-c
//
//  Created by 伍 威 on 15/1/9.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "olvalue.h"
#include "olpool.h"


static OLPool* pairPool;
static OLPool* valuePool;

void OLInit() {
}

OLValue* OLValueCreate() {
    if (!valuePool) {
        valuePool = OLPoolInit(sizeof(OLValue), 256);
    }
    OLValue* ret = OLPoolAlloc(valuePool);
    ret->ref = 1;
    ret->type = OL_NULL;
    return ret;
}

OLValue* OLValueCreateString(const char* begin, const char* end) {
    OLValue* ret = OLValueCreate();
    ret->type = OL_STRING;
    size_t length = end - begin;
    ret->stringValue = malloc(length + 1);
    strncpy(ret->stringValue, begin, length);
    return ret;
}

OLValue* OLValueCreateNumber(double number) {
    OLValue* ret = OLValueCreate();
    ret->type = OL_NUMBER;
    ret->numberValue = number;
    return ret;
}

OLValue* OLValueCreateBool(bool value) {
    OLValue* ret = OLValueCreate();
    ret->type = OL_BOOL;
    ret->boolValue = value;
    return ret;
}

OLValue* OLValueCreateArray() {
    OLValue* ret = OLValueCreate();
    ret->type = OL_ARRAY;
    ret->arrayValue = OLArrayCreate();
    return ret;
}

OLValue* OLValueCreateObject() {
    OLValue* ret = OLValueCreate();
    ret->type = OL_OBJECT;
    ret->arrayValue = OLArrayCreate();
    return ret;
}

OLValue* OLValueCreatePair(OLPair* pair) {
    OLValue* ret = OLValueCreate();
    ret->type = OL_PAIR;
    ret->pairValue = pair;
    return ret;
}


OLValue* OLValueRetain(OLValue* value) {
    if (value) {
        ++value->ref;
    }
    return value;
}

static void arrayCleanup(OLArray* array) {
    for (size_t i = 0; i < array->count; ++i) {
        OLValueRelease(array->buffer[i]);
    }
    OLArrayCleanup(array);
}

OLValue* OLValueRelease(OLValue* value) {
    if (value && --value->ref <= 0) {
        switch (value->type) {
            case OL_STRING:
                free(value->stringValue);
                break;
            case OL_ARRAY:
                arrayCleanup(value->arrayValue);
                break;
            case OL_OBJECT:
                arrayCleanup(value->arrayValue);
                break;
            case OL_PAIR:
                OLPairCleanup(value->pairValue);
                break;
            default:
                break;
        }
        OLPoolFree(valuePool, value);
    }
    return NULL;
}

OLPair* OLPairCreate() {
    if (!pairPool) {
        pairPool = OLPoolInit(sizeof(OLPair), 256);
    }
    OLPair* pair = OLPoolAlloc(pairPool);
    return pair;
}
OLPair* OLPairCleanup(OLPair* pair) {
    OLValueRelease(pair->key);
    OLValueRelease(pair->value);
    OLPoolFree(pairPool, pair);
    return NULL;
}
void OLValuePrint(OLValue* value) {
    switch (value->type) {
        case OL_NULL:
            printf("null");
            break;
        case OL_NUMBER:
            if ((long)value->numberValue == value->numberValue) {
                printf("%ld", (long)value->numberValue);
            } else {
                printf("%f", value->numberValue);
            }
            break;
        case OL_STRING:
            printf("%s", value->stringValue);
            break;
        case OL_PAIR:
            OLValuePrint(value->pairValue->key);
            printf(":");
            OLValuePrint(value->pairValue->value);
            break;
        case OL_ARRAY:
            printf("[");
            if (value->arrayValue->count) {
                OLValuePrint(OLArrayAtIndex(value->arrayValue, 0));
            }
            for (size_t i = 1; i < value->arrayValue->count; ++i) {
                printf(",");
                OLValuePrint(OLArrayAtIndex(value->arrayValue, i));
            }
            printf("]");
            break;
        case OL_OBJECT:
            printf("{");
            if (value->arrayValue->count) {
                OLValuePrint(OLArrayAtIndex(value->arrayValue, 0));
            }
            for (size_t i = 1; i < value->arrayValue->count; ++i) {
                printf(",");
                OLValuePrint(OLArrayAtIndex(value->arrayValue, i));
            }
            printf("}");
            break;
        case OL_BOOL:
            value->boolValue ? printf("true") : printf("false");
            break;
        default:
            break;
    }
}





