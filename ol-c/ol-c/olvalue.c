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

OLPool arrayPool;
OLPool pairPool;
OLPool valuePool;


size_t OLArrayCreate() {
    size_t i = OLPoolAlloc(&arrayPool);
    OLArray[i].buffer = malloc(sizeof(size_t) * 16);
    OLArray[i].cap = 16;
    OLArray[i].count = 0;
    return i;
}

size_t OLArrayCleanup(size_t index) {
    for (size_t i = 0; i < OLArray[index].count; ++i) {
        OLValueRelease(OLArray[index].buffer[i]);
    }
    free(OLArray[index].buffer);
    OLPoolFree(&arrayPool, index);
    return 0;
}

void OLArrayAppend(size_t array, size_t value) {
    if (OLArray[array].count == OLArray[array].cap) {
        OLArray[array].cap *= 2;
        OLArray[array].buffer = realloc(OLArray[array].buffer, sizeof(size_t) * OLArray[array].cap);
    }
    OLArray[array].buffer[OLArray[array].count] = value;
    ++OLArray[array].count;
}

size_t OLArrayAtIndex(size_t array, size_t index) {
    if (index < OLArray[array].count) {
        return OLArray[array].buffer[index];
    } else {
        return 0;
    }
}

void OLInit() {
    OLPoolInit(&arrayPool, sizeof(struct OLArray), 256, (void**)&OLArray);
    OLPoolInit(&valuePool, sizeof(struct OLValue), 256, (void**)&OLValue);
    OLPoolInit(&pairPool, sizeof(struct OLPair), 256, (void**)&OLPair);
}

size_t OLValueCreate() {
    size_t ret = OLPoolAlloc(&valuePool);
    OLValue[ret].ref = 1;
    OLValue[ret].type = OL_NULL;
    return ret;
}

size_t OLValueCreateString(const char* begin, const char* end) {
    size_t ret = OLValueCreate();
    OLValue[ret].type = OL_STRING;
    size_t length = end - begin;
    OLValue[ret].stringValue = malloc(length + 1);
    strncpy(OLValue[ret].stringValue, begin, length);
    return ret;
}

size_t OLValueCreateNumber(double number) {
    size_t ret = OLValueCreate();
    OLValue[ret].type = OL_NUMBER;
    OLValue[ret].numberValue = number;
    return ret;
}

size_t OLValueCreateBool(bool value) {
    size_t ret = OLValueCreate();
    OLValue[ret].type = OL_BOOL;
    OLValue[ret].boolValue = value;
    return ret;
}

size_t OLValueCreateArray() {
    size_t ret = OLValueCreate();
    OLValue[ret].type = OL_ARRAY;
    OLValue[ret].pointerValue = OLArrayCreate();
    return ret;
}

size_t OLValueCreateObject() {
    size_t ret = OLValueCreate();
    OLValue[ret].type = OL_OBJECT;
    OLValue[ret].pointerValue = OLArrayCreate();
    return ret;
}

size_t OLValueCreatePair(size_t pair) {
    size_t ret = OLValueCreate();
    OLValue[ret].type = OL_PAIR;
    OLValue[ret].pointerValue = pair;
    return ret;
}


size_t OLValueRetain(size_t value) {
    if (value) {
        ++OLValue[value].ref;
    }
    return value;
}

size_t OLValueRelease(size_t value) {
    if (value && --OLValue[value].ref <= 0) {
        switch (OLValue[value].type) {
            case OL_STRING:
                free(OLValue[value].stringValue);
                break;
            case OL_ARRAY:
                OLArrayCleanup(OLValue[value].pointerValue);
                break;
            case OL_OBJECT:
                OLArrayCleanup(OLValue[value].pointerValue);
                break;
            case OL_PAIR:
                OLPairCleanup(OLValue[value].pointerValue);
                break;
            default:
                break;
        }
        OLPoolFree(&valuePool, value);
    }
    return 0;
}

size_t OLPairCreate() {
    size_t pair = OLPoolAlloc(&pairPool);
    return pair;
}
size_t OLPairCleanup(size_t pair) {
    OLValueRelease(OLPair[pair].key);
    OLValueRelease(OLPair[pair].value);
    OLPoolFree(&pairPool, pair);
    return 0;
}
void OLValuePrint(size_t index) {
    struct OLValue* value = OLValue + index;
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
            OLValuePrint(OLPair[value->pointerValue].key);
            printf(":");
            OLValuePrint(OLPair[value->pointerValue].value);
            break;
        case OL_ARRAY:
            printf("[");
            if (OLArray[value->pointerValue].count) {
                OLValuePrint(OLArrayAtIndex(value->pointerValue, 0));
            }
            for (size_t i = 1; i < OLArray[value->pointerValue].count; ++i) {
                printf(",");
                OLValuePrint(OLArrayAtIndex(value->pointerValue, i));
            }
            printf("]");
            break;
        case OL_OBJECT:
            printf("{");
            if (OLArray[value->pointerValue].count) {
                OLValuePrint(OLArrayAtIndex(value->pointerValue, 0));
            }
            for (size_t i = 1; i < OLArray[value->pointerValue].count; ++i) {
                printf(",");
                OLValuePrint(OLArrayAtIndex(value->pointerValue, i));
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

struct OLArray* OLArray;
struct OLPair* OLPair;
struct OLValue* OLValue;




