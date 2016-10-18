//
//  dijkstra.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef dijkstra_h
#define dijkstra_h

#include "../point/point.h"
#include "../env/env.h"
#include "../graph/graph.h"

typedef struct dijkstra_namespace {
    graph_t* (*const construct)(int invert);
    void     (*const destruct)(graph_t* g);
    void     (*const dijkstra)(graph_t* g, point_t* end, point_t* start);
    path_node_t* (*const bresenham)(point_t* end, point_t* start);
} dijkstra_namespace;
extern dijkstra_namespace const dijkstraAPI;

#endif /* dijkstra_h */
