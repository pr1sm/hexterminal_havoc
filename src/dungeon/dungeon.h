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
#include "../graph/graph.h"

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80
#define ROCK_MAX 255
#define ROCK_HARD 171
#define ROCK_MED  85
#define ROCK_SOFT 1

typedef struct dungeon_t dungeon_t;
struct dungeon_t {
    tile_t*** tiles;
    room_t**  rooms;
    int       room_size;
    graph_t* tunnel_map;
    graph_t* non_tunnel_map;
    void (*update_path_maps)(dungeon_t* d);
    void (*print)(dungeon_t* d, int mode);
    void (*printn)(dungeon_t* d, int mode);
    void (*load)(dungeon_t* d);
    void (*save)(dungeon_t* d);
};

typedef struct dungeon_namespace {
    dungeon_t* (*const get_dungeon)();
    dungeon_t* (*const construct)();
    dungeon_t* (*const move_floors)();
    void (*const destruct)(dungeon_t* d);
    void (*const generate)(dungeon_t* d);
    void (*const rand_point)(dungeon_t* d, point_t* p);
} dungeon_namespace;
extern dungeon_namespace const dungeonAPI;

#endif /* dungeon_h */
