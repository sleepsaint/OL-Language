//
//  olarray.c
//  ol-c
//
//  Created by 伍 威 on 15/1/29.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//
#include <stdlib.h>
#include "olarray.h"
#include "olpool.h"


static OLPool* arrayPool;

void OLArrayInit(OLArray* array) {
    array->buffer = malloc(sizeof(void*) * 16);
    array->cap = 16;
    array->count = 0;
}

OLArray* OLArrayCreate() {
    if (!arrayPool) {
        arrayPool = OLPoolInit(sizeof(OLArray), 256);
    }
    OLArray* array = OLPoolAlloc(arrayPool);
    OLArrayInit(array);
    return array;
}

OLArray* OLArrayCleanup(OLArray* array) {
    free(array->buffer);
    OLPoolFree(arrayPool, array);
    return 0;
}

void OLArrayAppend(OLArray* array, void* value) {
    if (array->count == array->cap) {
        array->cap *= 2;
        array->buffer = realloc(array->buffer, sizeof(size_t) * array->cap);
    }
    array->buffer[array->count] = value;
    ++array->count;
}

void* OLArrayAtIndex(OLArray* array, size_t index) {
    if (index < array->count) {
        return array->buffer[index];
    } else {
        return NULL;
    }
}
