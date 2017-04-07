//
//  util.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include "util.h"
#include "../point/point.h"
#include "../dungeon/dungeon.h"

int util::point_to_index(point* p) {
    // since outer rows and cols aren't being used
    // subtract one from both so the index starts at 0
    return ((p->y - 1) * (DUNGEON_WIDTH-2)) + (p->x - 1);
}

void util::index_to_point(int index, point* p) {
    p->x = (index % (DUNGEON_WIDTH-2))+1;
    p->y = (index / (DUNGEON_WIDTH-2))+1;
}