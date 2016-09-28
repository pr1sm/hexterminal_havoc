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
#include "../room/room.h"

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80
#define ROCK_MAX 255
#define ROCK_HARD 230
#define ROCK_MED  130
#define ROCK_SOFT 30

typedef struct dungeon_t {
    tile_t*** tiles;
    room_t**  rooms;
    int       room_size;
} dungeon_t;

typedef struct dungeon_namespace {
    dungeon_t* (*const construct)();
    void (*const destruct)(dungeon_t* d);
    void (*const generate)(dungeon_t* d);
    void (*const update_path_maps)(dungeon_t* d);
    void (*const check_room_intercept)(dungeon_t* d, point_t* point);
    void (*const print)(dungeon_t* d, int mode);
    void (*const load)(dungeon_t* d);
    void (*const save)(dungeon_t* d);
    // should be moved to playerAPI:
    void (*const set_player_pos)(dungeon_t* d, point_t* p);
    point_t* (*const get_player_pos)();
} dungeon_namespace;
extern dungeon_namespace const dungeonAPI;

#endif /* dungeon_h */
