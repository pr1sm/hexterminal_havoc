//
//  pathfinder.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef pathfinder_h
#define pathfinder_h

#include "../env/env.h"
#include "../point/point.h"
#include "../graph/graph.h"

typedef struct pathfinder_namespace {
    graph_t* (*const construct)(int invert);
    void     (*const destruct)();
    void     (*const pathfind)(graph_t* g, point_t* start, point_t* end);
} pathfinder_namespace;
extern pathfinder_namespace const pathfinderAPI;


#endif /* pathfinder_h */
