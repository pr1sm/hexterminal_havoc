//
//  tile.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>

#include "tile.h"
#include "../env/env.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"

#define BORDER_CHAR_DEBUG '%'
#define BORDER_CHAR_NORM ' '
#define BORDER_CHAR (DEBUG_MODE ? BORDER_CHAR_DEBUG : BORDER_CHAR_NORM)
#define DEFAULT_CHAR_NORM ' '
#define DEFAULT_CHAR_DEBUG '?'
#define DEFAULT_CHAR (DEBUG_MODE ? DEFAULT_CHAR_DEBUG : DEFAULT_CHAR_NORM)
#define ROCK_CHAR ' '
#define ROOM_CHAR '.'
#define PATH_CHAR '#'
#define PC_CHAR '@'


static tile_t* construct(uint8_t x, uint8_t y) {
    tile_t* t = (tile_t*)malloc(sizeof(tile_t));
    point_t* location = pointAPI.construct(x, y);
    t->location = location;
    t->content = tc_UNSET;
    t->rock_hardness = 0;
    t->changes = (tile_t*)malloc(sizeof(tile_t));
    tileAPI.update_dist(t, 255);
    tileAPI.update_dist_tunnel(t, 255);
    return t;
}

static void destruct(tile_t* tile) {
    free(tile->changes);
    free(tile->location);
    free(tile);
}

static void update_hardness(tile_t* tile, uint8_t value) {
    tile->rock_hardness = value;
    tile->changes->rock_hardness = value;
}

static void update_content(tile_t* tile, tile_content value) {
    tile->content = value;
    tile->changes->content = value;
}

static void update_dist(tile_t* tile, uint8_t value) {
    tile->dist = value;
    tile->changes->dist = value;
}

static void update_dist_tunnel(tile_t* tile, uint8_t value) {
    tile->dist_tunnel = value;
    tile->changes->dist_tunnel = value;
}

static void propose_update_hardness(tile_t* tile, uint8_t value) {
    tile->changes->rock_hardness = value;
}

static void propose_update_content(tile_t* tile, tile_content value) {
    tile->changes->content = value;
}

static void propose_update_dist(tile_t* tile, uint8_t value) {
    tile->changes->dist = value;
}

static void propose_update_dist_tunnel(tile_t* tile, uint8_t value) {
    tile->changes->dist_tunnel = value;
}

static void commit_updates(tile_t* tile) {
    tile->rock_hardness = tile->changes->rock_hardness;
    tile->content       = tile->changes->content;
    tile->dist          = tile->changes->dist;
    tile->dist_tunnel   = tile->changes->dist_tunnel;
}

static int are_changes_proposed(tile_t* tile) {
    return (tile->rock_hardness != tile->changes->rock_hardness) ||
           (tile->content != tile->changes->content) ||
           (tile->dist != tile->changes->dist) ||
           (tile->dist_tunnel != tile->changes->dist_tunnel);
}

static char char_for_content(tile_t* tile, int mode) {
    if(mode == PM_DUNGEON) {
        point_t* player_pos = dungeonAPI.get_player_pos();
        if(pointAPI.distance(tile->location, player_pos) == 0) {
            return PC_CHAR;
        }
        return tile->content == tc_BORDER ? BORDER_CHAR :
        tile->content == tc_ROCK   ? ROCK_CHAR   :
        tile->content == tc_ROOM   ? ROOM_CHAR   :
        tile->content == tc_PATH   ? PATH_CHAR   : DEFAULT_CHAR ;
    } else if(mode == PM_ROOM_PATH_MAP || mode == PM_TUNN_PATH_MAP) {
        uint8_t val = mode == 1 ? tile->dist : tile->dist_tunnel;
        if(val < 10) {
            return val + '0';
        } else if(val < 36) {
            return val - 10 + 'a';
        } else if(val < 62) {
            return val - 36 + 'A';
        } else {
            return tileAPI.char_for_content(tile, PM_DUNGEON);
        }
    }
    return DEFAULT_CHAR;
}

static void import_tile(tile_t* tile, unsigned char value, int room) {
    tileAPI.propose_update_hardness(tile, value);
    if(value == 255) {
        tileAPI.propose_update_content(tile, tc_BORDER);
    } else if(value == 0) {
        tileAPI.propose_update_content(tile, tc_PATH);
    } else {
        tileAPI.propose_update_content(tile, tc_ROCK);
    }
    
    if(room) {
        tileAPI.propose_update_content(tile, tc_ROOM);
    }
    tileAPI.propose_update_dist(tile, 255);
    tileAPI.propose_update_dist_tunnel(tile, 255);
    
    tileAPI.commit_updates(tile);
}

tile_namespace const tileAPI = {
    construct,
    destruct,
    update_hardness,
    update_content,
    update_dist,
    update_dist_tunnel,
    propose_update_hardness,
    propose_update_content,
    propose_update_dist,
    propose_update_dist_tunnel,
    commit_updates,
    are_changes_proposed,
    char_for_content,
    import_tile
};
