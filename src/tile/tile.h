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
#include "../env/env.h"

#define PM_DUNGEON 0
#define PM_ROOM_PATH_MAP 1
#define PM_TUNN_PATH_MAP 2

typedef enum {
    tc_UNSET, tc_BORDER, tc_ROCK, tc_ROOM, tc_PATH
} tile_content;

typedef struct tile_t tile_t;
struct tile_t {
    point_t* location;
    uint8_t rock_hardness;
    uint8_t dist;
    uint8_t dist_tunnel;
    tile_content content;
    tile_t* changes;
};

typedef struct {
    tile_t* (*const construct)(uint8_t x, uint8_t y);
    void    (*const destruct)(tile_t* tile);
    void    (*const update_hardness)(tile_t* tile, uint8_t value);
    void    (*const update_content)(tile_t* tile, tile_content value);
    void    (*const update_dist)(tile_t* tile, uint8_t value);
    void    (*const update_dist_tunnel)(tile_t* tile, uint8_t value);
    void    (*const propose_update_hardness)(tile_t* tile, uint8_t value);
    void    (*const propose_update_content)(tile_t* tile, tile_content value);
    void    (*const propose_update_dist)(tile_t* tile, uint8_t value);
    void    (*const propose_update_dist_tunnel)(tile_t* tile, uint8_t value);
    void    (*const commit_updates)(tile_t* tile);
    int     (*const are_changes_proposed)(tile_t* tile);
    char    (*const char_for_content)(tile_t* tile, int mode);
    void    (*const import_tile)(tile_t* tile, uint8_t value, int room);
} tile_namespace;
extern tile_namespace const tileAPI;

#endif /* tile_h */