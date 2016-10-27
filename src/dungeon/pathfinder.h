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
#include "dungeon.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct pathfinder_namespace {
    graph_t* (*const construct)(dungeon_t* d, int tunnel);
    void     (*const destruct)(graph_t* g);
    int      (*const generate_pathmap)(graph_t* g, dungeon_t* d, point_t* start, int tunnel);
} pathfinder_namespace;
extern pathfinder_namespace const pathfinderAPI;
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* pathfinder_h */
