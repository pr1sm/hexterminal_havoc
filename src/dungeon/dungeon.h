//
//  dungeon.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef dungeon_h
#define dungeon_h

#include "../tile/tile.h"
#include "../point/point.h"
#include "../env/env.h"

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80
#define ROCK_MAX 255
#define ROCK_HARD 230
#define ROCK_MED  130
#define ROCK_SOFT 30

extern tile_t*** _dungeon_array;

typedef struct dungeon_namespace {
    void (*const construct)();
    void (*const destruct)();
    void (*const generate)();
    void (*const update_path_maps)();
    void (*const check_room_intercept)(point_t* point);
    void (*const print)(int mode);
    void (*const load)();
    void (*const save)();
    // should be moved to playerAPI:
    void (*const set_player_pos)(point_t* p);
    point_t* (*const get_player_pos)();
} dungeon_namespace;
extern dungeon_namespace const dungeonAPI;

#endif /* dungeon_h */
