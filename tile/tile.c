//
//  tile.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "tile.h"

tile_t* tile_construct(int x, int y) {
    point_t* location = point.construct(x, y);
    return tile.construct_loc(location);
}

tile_t* tile_construct_loc(point_t* location) {
    tile_t* t = (tile_t*)malloc(sizeof(tile_t));
    t->location = location;
    t->content = tc_UNSET;
    t->rock_hardness = 0;
    
    return t;
}

int tile_destruct(tile_t* tile) {
    free(tile);
    return 0;
}

int update_hardness(tile_t* tile, int value) {
    tile->rock_hardness = value;
    return 0;
}

int update_content(tile_t* tile, tile_content value) {
    tile->content = value;
    return 0;
}

tile_namespace const tile = { tile_construct, tile_construct_loc, tile_destruct, update_hardness, update_content };
