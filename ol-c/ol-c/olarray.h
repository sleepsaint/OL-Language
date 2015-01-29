//
//  olarray.h
//  ol-c
//
//  Created by 伍 威 on 15/1/29.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#ifndef __ol_c__olarray__
#define __ol_c__olarray__

#include <stdio.h>

typedef struct OLArray OLArray;

struct OLArray {
    void** buffer;
    size_t cap;
    size_t count;
};

OLArray* OLArrayCreate();
OLArray* OLArrayCleanup(OLArray* array);
void OLArrayAppend(OLArray* array, void* value);
void* OLArrayAtIndex(OLArray* array, size_t index);
void OLArrayInit(OLArray* array);

#endif /* defined(__ol_c__olarray__) */
