//
//  olvalue.c
//  ol-c
//
//  Created by 伍 威 on 15/1/9.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <stdlib.h>
#include "olvalue.h"

OLArray* createOLArray() {
    OLArray* array = malloc(sizeof(OLArray));
    array->cap = 16;
    array->count = 0;
    array->buffer = malloc(sizeof(OLValue) * array->cap);
    return array;
}

inline OLArray* cleanupOLArray(OLArray* array) {
    for (size_t i = 0; i < array->count; ++i) {
        cleanupOLValue(array->buffer + i);
    }
    free(array->buffer);
    return NULL;
}
inline void appendOLArray(OLArray* array, OLValue value) {
    if (array->count == array->cap) {
        array->cap *= 2;
        array->buffer = realloc(array->buffer, sizeof(OLValue) * array->cap);
    }
    array->buffer[array->count] = value;
    ++array->count;
}

OLValue nullValue;
inline OLValue valueAtIndex(OLArray* array, size_t index) {
    if (index < array->count) {
        return array->buffer[index];
    } else {
        return nullValue;
    }
}

OLValue* cleanupOLValue(OLValue* value) {
    return NULL;
}