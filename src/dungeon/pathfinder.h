//
//  pathfinder.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef pathfinder_h
#define pathfinder_h

#include "../graph/graph.h"
#include "../point/point.h"
#include "dungeon.h"

class pathfinder {
private:
    static int hardness_to_weight(int hardness);
    static void update_tiles(graph* g, dungeon* d, int tunnel);
    
public:
    static graph* construct(dungeon* d, int tunnel);
    static void   destruct(graph* g);
    static int    generate_pathmap(graph* g, dungeon* d, point* start, int tunnel);
};

#endif /* pathfinder_h */
