//
//  corridor.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef corridor_h
#define corridor_h

#include "../env/env.h"
#include "../point/point.h"
#include "../graph/graph.h"
#include "dungeon.h"

typedef struct corridor_namespace {
    graph_t* (*const construct)(dungeon_t* d, int invert);
    void     (*const destruct)(graph_t* g);
    void     (*const pathfind)(graph_t* g, dungeon_t* d, point_t* start, point_t* end);
} corridor_namespace;
extern corridor_namespace const corridorAPI;


#endif /* corridor_h */
