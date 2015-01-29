//
//  olpool.h
//  ol-c
//
//  Created by 伍威 on 15/1/29.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#ifndef __ol_c__olpool__
#define __ol_c__olpool__

#include <stdio.h>

struct OLPool {
    void* node;
    size_t* freeIndex;
    size_t cap;
    size_t count;
    size_t nodeSize;
    void** holder;
};

typedef struct OLPool OLPool;

void OLPoolInit(OLPool* pool, size_t nodeSize, size_t capSize, void** holder);
size_t OLPoolAlloc(OLPool* pool);
size_t OLPoolFree(OLPool* pool, size_t index);

#endif /* defined(__ol_c__olpool__) */
