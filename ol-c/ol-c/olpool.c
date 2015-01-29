//
//  olpool.c
//  ol-c
//
//  Created by 伍威 on 15/1/29.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <stdlib.h>
#include "olpool.h"

void OLPoolInit(OLPool* pool, size_t nodeSize, size_t capSize, void** holder) {
    pool->node = malloc(nodeSize * capSize);
    pool->freeIndex = malloc(sizeof(size_t) * capSize);
    pool->cap = capSize;
    pool->count = 1;
    pool->nodeSize = nodeSize;
    for (size_t i = 0; i < capSize; ++i) {
        pool->freeIndex[i] = i;
    }
    pool->holder = holder;
    *holder = pool->node;
}

size_t OLPoolAlloc(OLPool* pool) {
    if (pool->count == pool->cap) {
        size_t cap = pool->cap * 2;
        pool->node = realloc(pool->node, pool->nodeSize * cap);
        pool->freeIndex = realloc(pool->freeIndex, sizeof(size_t) * cap);
        for (size_t i = pool->cap; i < cap; ++i) {
            pool->freeIndex[i] = i;
        }
        *(pool->holder) = pool->node;
        pool->cap = cap;
    }
    
    return pool->freeIndex[pool->count++];
}

size_t OLPoolFree(OLPool* pool, size_t index) {
    pool->freeIndex[--pool->count] = index;
    return 0;
}
