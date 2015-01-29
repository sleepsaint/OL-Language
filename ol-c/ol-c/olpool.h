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


typedef struct OLPool OLPool;

OLPool* OLPoolInit(size_t nodeSize, size_t capSize);
void* OLPoolAlloc(OLPool* pool);
void* OLPoolFree(OLPool* pool, void* p);

#endif /* defined(__ol_c__olpool__) */
