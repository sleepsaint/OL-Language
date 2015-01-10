//
//  olvalue.c
//  ol-c
//
//  Created by 伍 威 on 15/1/9.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <stdlib.h>
#include "olvalue.h"

OLValue OLNullValue;

OLArray* OLArrayCreate() {
    OLArray* array = malloc(sizeof(OLArray));
    array->cap = 16;
    array->count = 0;
    array->buffer = malloc(sizeof(OLValue) * array->cap);
    return array;
}

OLArray* OLArrayCleanup(OLArray* array) {
    for (size_t i = 0; i < array->count; ++i) {
        OLValueCleanup(array->buffer + i);
    }
    free(array->buffer);
    free(array);
    return NULL;
}
OLValue* OLArrayAppend(OLArray* array) {
    if (array->count == array->cap) {
        array->cap *= 2;
        array->buffer = realloc(array->buffer, sizeof(OLValue) * array->cap);
    }
    ++array->count;
    return array->buffer + array->count - 1;
}

OLValue OLArrayAtIndex(OLArray* array, size_t index) {
    if (index < array->count) {
        return array->buffer[index];
    } else {
        return OLNullValue;
    }
}

OLValue* OLValueCleanup(OLValue* value) {
    switch (value->type) {
        case OL_STRING:
            free(value->stringValue);
            break;
        case OL_ARRAY:
            OLArrayCleanup(value->arrayValue);
            break;
        case OL_OBJECT:
            OLArrayCleanup(value->arrayValue);
            break;
            case OL_PAIR:
        OLPairCleanup(value->pairValue);
            break;
        default:
            break;
    }
    return NULL;
}

OLPair* OLPairCreate() {
    OLPair* pair = malloc(sizeof(OLPair));
    return pair;
}
OLPair* OLPairCleanup(OLPair* pair) {
    OLValueCleanup(&pair->key);
    OLValueCleanup(&pair->value);
    free(pair);
    return NULL;
}
void OLValuePrint(OLValue value) {
    switch (value.type) {
        case OL_NULL:
            printf("null");
            break;
        case OL_NUMBER:
            if ((long)value.numberValue == value.numberValue) {
                printf("%ld", (long)value.numberValue);
            } else {
                printf("%f", value.numberValue);
            }
            break;
        case OL_STRING:
            printf("%s", value.stringValue);
            break;
        case OL_PAIR:
            OLValuePrint(value.pairValue->key);
            printf(":");
            OLValuePrint(value.pairValue->value);
            break;
        case OL_ARRAY:
            printf("[");
            if (value.arrayValue->count) {
                OLValuePrint(OLArrayAtIndex(value.arrayValue, 0));
            }
            for (size_t i = 1; i < value.arrayValue->count; ++i) {
                printf(",");
                OLValuePrint(OLArrayAtIndex(value.arrayValue, i));
            }
            printf("]");
            break;
        case OL_OBJECT:
            printf("{");
            if (value.arrayValue->count) {
                OLValuePrint(OLArrayAtIndex(value.arrayValue, 0));
            }
            for (size_t i = 1; i < value.arrayValue->count; ++i) {
                printf(",");
                OLValuePrint(OLArrayAtIndex(value.arrayValue, i));
            }
            printf("}");
            break;
        case OL_BOOL:
            value.boolValue ? printf("true") : printf("false");
            break;
        default:
            break;
    }
}
