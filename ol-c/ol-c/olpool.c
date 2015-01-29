//
//  olpool.c
//  ol-c
//
//  Created by 伍威 on 15/1/29.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <stdlib.h>
#include "olpool.h"
#include "olarray.h"

struct OLPool {
    OLArray array;
    void** freeNode;
    size_t blockSize;
    size_t cap;
    size_t count;
    size_t incCap;
    size_t nodeSize;
};

OLPool* OLPoolInit(size_t nodeSize, size_t capSize) {
    OLPool* pool = malloc(sizeof(OLPool));
    OLArrayInit(&pool->array);
    pool->blockSize = nodeSize * capSize;
    void** mem = malloc(pool->blockSize);
    OLArrayAppend(&pool->array, mem);
    pool->freeNode = malloc(sizeof(void*) * capSize);
    pool->cap = capSize;
    pool->count = 0;
    pool->incCap = capSize;
    pool->nodeSize = nodeSize;
    for (size_t i = 0; i < capSize; ++i) {
        pool->freeNode[i] = (void*)((size_t)mem + i * nodeSize);
    }
    return pool;
}

void* OLPoolAlloc(OLPool* pool) {
    if (pool->count == pool->cap) {
        size_t cap = pool->cap + pool->incCap;
        void* mem = malloc(pool->blockSize);
        OLArrayAppend(&pool->array, mem);
        pool->freeNode = realloc(pool->freeNode, sizeof(void*) * cap);
        for (size_t i = 0; i < pool->incCap; ++i) {
            pool->freeNode[i + pool->cap] = (void*)((size_t)mem + i * pool->nodeSize);;
        }
        pool->cap = cap;
    }
    
    return pool->freeNode[pool->count++];
}

void* OLPoolFree(OLPool* pool, void* p) {
    pool->freeNode[--pool->count] = p;
    return NULL;
}
