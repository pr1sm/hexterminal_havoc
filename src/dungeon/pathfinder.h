//
//  pathfinder.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef pathfinder_h
#define pathfinder_h

#include "../graph/graph.h"
#include "../point/point.h"

typedef struct pathfinder_namespace {
    graph_t* (*const construct)(int tunnel);
    void     (*const destruct)(graph_t* g);
    void     (*const generate_pathmap)(graph_t* g, point_t* start, int tunnel);
} pathfinder_namespace;
extern pathfinder_namespace const pathfinderAPI;

#endif /* pathfinder_h */
