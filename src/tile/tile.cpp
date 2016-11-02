//
//  tile.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>
#include <ncurses.h>


#include "tile.h"
#include "../env/env.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"
#include "../character/character.h"
#include "../character/character_store.h"

#define BORDER_CHAR_DEBUG '%'
#define BORDER_CHAR_NORM ' '
#define BORDER_CHAR (env_constants::DEBUG_MODE ? BORDER_CHAR_DEBUG : BORDER_CHAR_NORM)
#define DEFAULT_CHAR_NORM ' '
#define DEFAULT_CHAR_DEBUG '?'
#define DEFAULT_CHAR (env_constants::DEBUG_MODE ? DEFAULT_CHAR_DEBUG : DEFAULT_CHAR_NORM)
#define ROCK_CHAR ' '
#define ROOM_CHAR '.'
#define PATH_CHAR '#'
#define PC_CHAR '@'
#define UPSTR_CHAR '<'
#define DNSTR_CHAR '>'

tile::tile(uint8_t x, uint8_t y) {
    this->location = new point(x, y);
    this->content = tc_UNSET;
    this->rock_hardness = 0;
    this->dist = 255;
    this->dist_tunnel = 255;
    this->last_known_content = ROCK_CHAR;
    this->change_location = new point(x, y);
    this->change_content = tc_UNSET;
    this->change_rock_hardness = 0;
    this->change_dist = 255;
    this->change_dist_tunnel = 255;
    this->change_last_known_content = ROCK_CHAR;
}

tile::tile(uint8_t value, bool room) {
    change_rock_hardness = value;
    if(value == 255) {
        change_content = tc_BORDER;
    } else if(value == 0) {
        change_content = tc_PATH;
    } else {
        change_content = tc_ROCK;
    }
    
    if(room) {
        change_content = tc_ROOM;
    }
    
    change_dist = 255;
    change_dist_tunnel = 255;
    
    rock_hardness = change_rock_hardness;
    content       = change_content;
    dist          = change_dist;
    dist_tunnel   = change_dist_tunnel;
    if(content == tc_BORDER && last_known_content != BORDER_CHAR) {
        last_known_content = BORDER_CHAR;
    }
}

tile::~tile() {
    delete location;
    delete change_location;
}

void tile::update_hardness(uint8_t value) {
    rock_hardness = value;
    change_rock_hardness = value;
}

void tile::update_content(tile_content value) {
    content = value;
    change_content = value;
    if(content == tc_BORDER && last_known_content != BORDER_CHAR) {
        last_known_content = BORDER_CHAR;
    }
}

void tile::update_dist(uint8_t value) {
    dist = value;
    change_dist = value;
}

void tile::update_dist_tunnel(uint8_t value) {
    dist_tunnel = value;
    change_dist_tunnel = value;
}

void tile::propose_update_hardness(uint8_t value) {
    change_rock_hardness = value;
}

void tile::propose_update_content(tile_content value) {
    change_content = value;
}

void tile::propose_update_dist(uint8_t value) {
    change_dist = value;
}

void tile::propose_update_dist_tunnel(uint8_t value) {
    change_dist_tunnel = value;
}

void tile::commit_updates() {
    rock_hardness = change_rock_hardness;
    content       = change_content;
    dist          = change_dist;
    dist_tunnel   = change_dist_tunnel;
    if(content == tc_BORDER && last_known_content != BORDER_CHAR) {
        last_known_content = BORDER_CHAR;
    }
}

int tile::are_changes_proposed() {
    return (rock_hardness != change_rock_hardness) ||
           (content != change_content) ||
           (dist != change_dist) ||
           (dist_tunnel != change_dist_tunnel);
}

char tile::char_for_content(int mode) {
    if(mode == PM_DUNGEON) {
        character* pc = character::get_pc();
        point* pc_pos = pc->_position;
        // tile is outside of pc's light
        if(location->distance_to(pc_pos) > 3) {
            return last_known_content;
        }
        int npc_idx = character_store::contains_npc(location);
        last_known_content = content == tc_BORDER ? BORDER_CHAR :
                             content == tc_ROCK   ? ROCK_CHAR   :
                             content == tc_ROOM   ? ROOM_CHAR   :
                             content == tc_PATH   ? PATH_CHAR   :
                             content == tc_UPSTR  ? UPSTR_CHAR  :
                             content == tc_DNSTR  ? DNSTR_CHAR  : DEFAULT_CHAR ;
        if(npc_idx != -1) {
            return character_store::get_char_for_npc_at_index(npc_idx) | A_BOLD;
        }
        return last_known_content | A_BOLD;
    } else if(mode == PM_ROOM_PATH_MAP || mode == PM_TUNN_PATH_MAP) {
        uint8_t val = (mode == 1 ? dist : dist_tunnel);
        if(val < 10) {
            return val + '0';
        } else if(val < 36) {
            return val - 10 + 'a';
        } else if(val < 62) {
            return val - 36 + 'A';
        } else {
            return this->char_for_content(PM_DUNGEON);
        }
    }
    return DEFAULT_CHAR;
}

uint8_t tile::npc_tunnel() {
    uint8_t new_hardness = rock_hardness - 85 > 0 ? rock_hardness - 85 : 0;
    this->update_hardness(new_hardness);
    return new_hardness;
}
