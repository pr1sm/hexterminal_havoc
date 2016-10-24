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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define PM_DUNGEON 0
#define PM_ROOM_PATH_MAP 1
#define PM_TUNN_PATH_MAP 2

typedef enum {
    tc_UNSET, tc_BORDER, tc_ROCK, tc_ROOM, tc_PATH, tc_UPSTR, tc_DNSTR
} tile_content;

typedef struct tile_t tile_t;
struct tile_t {
    // variables
    point_t* location;
    uint8_t rock_hardness;
    uint8_t dist;
    uint8_t dist_tunnel;
    tile_content content;
    tile_t* changes;
    
    // functions
    void    (*update_hardness)(tile_t* tile, uint8_t value);
    void    (*update_content)(tile_t* tile, tile_content value);
    void    (*update_dist)(tile_t* tile, uint8_t value);
    void    (*update_dist_tunnel)(tile_t* tile, uint8_t value);
    void    (*propose_update_hardness)(tile_t* tile, uint8_t value);
    void    (*propose_update_content)(tile_t* tile, tile_content value);
    void    (*propose_update_dist)(tile_t* tile, uint8_t value);
    void    (*propose_update_dist_tunnel)(tile_t* tile, uint8_t value);
    void    (*commit_updates)(tile_t* tile);
    int     (*are_changes_proposed)(tile_t* tile);
    char    (*char_for_content)(tile_t* tile, int mode);
    uint8_t (*npc_tunnel)(tile_t* tile);
};

typedef struct {
    tile_t* (*const construct)(uint8_t x, uint8_t y);
    void    (*const destruct)(tile_t* tile);
    void    (*const import_tile)(tile_t* tile, uint8_t value, int room);
} tile_namespace;
extern tile_namespace const tileAPI;
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* tile_h */
