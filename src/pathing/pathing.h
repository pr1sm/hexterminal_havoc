//
//  pathing.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef pathing_h
#define pathing_h

#include "../point/point.h"
#include "../env/env.h"
#include "../graph/graph.h"

class pathing {
public:
    static int        dijkstra(graph* g, point* end, point* start);
    static path_node* bresenham(point* end, point* start);
};

#endif /* pathing_h */
