//
//  corridor.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef corridor_h
#define corridor_h

#include "../env/env.h"
#include "../point/point.h"
#include "../graph/graph.h"
#include "dungeon.h"

class corridor {
private:
    static void place_path(graph* g, dungeon* d, point* b);
    static void check_room_intercept(dungeon* d, point* p);
public:
    static graph* construct(dungeon* d, int invert);
    static void   destruct(graph* g);
    static void   pathfind(graph* g, dungeon* d, point* start, point* end);
};

#endif /* corridor_h */
