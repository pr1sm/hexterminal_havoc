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

typedef struct corridor_namespace {
    graph_t* (*const construct)(int invert);
    void     (*const destruct)();
    void     (*const pathfind)(graph_t* g, point_t* start, point_t* end);
} corridor_namespace;
extern corridor_namespace const corridorAPI;


#endif /* corridor_h */
