//
//  tile.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef tile_h
#define tile_h

#include "../point/point.h"

typedef enum {
    tc_UNSET, tc_BORDER, tc_ROCK, tc_ROOM
} tile_content;

typedef struct tile_t {
    point_t* location;
    int rock_hardness;
    tile_content content;
} tile_t;

typedef struct {
    tile_t* (*const construct)(int x, int y);
    tile_t* (*const construct_loc)(point_t* location);
    int (*const destruct)(tile_t* tile);
    int (*const update_hardness)(tile_t* tile, int value);
    int (*const update_content)(tile_t* tile, tile_content value);
} tile_namespace;
extern tile_namespace const tile;

#endif /* tile_h */
