//
//  character_store.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "character_store.h"
#include "character.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"
#include "../events/event_queue.h"

// Temporary for player movement
#include "../graph/graph.h"
#include "../dungeon/pathfinder.h"
#include "../dijkstra/dijkstra.h"


static character_t** _characters = NULL;
static int _characters_size = 0;
static int _characters_count = 0;

// Temporary for player movement!
static graph_t* player_path = NULL;
static void temp_setup_player_movement();

static void setup_impl() {
    // TODO: change to get by CLI
    int nummon = 6;
    int i;
    dungeon_t* d = dungeonAPI.get_dungeon();
    _characters_size = nummon + 1; // num of monsters + pc
    _characters = (character_t**) calloc(_characters_size, sizeof(*_characters));
    _characters_count = _characters_size;
    _characters[0] = characterAPI.get_pc();
    temp_setup_player_movement();
    for(i = 1; i < _characters_size; i++) {
        point_t* p = dungeonAPI.rand_point(d);
        character_t* npc = characterAPI.construct(NPC, p);
        // TODO: Setup destination point
        _characters[i] = npc;
    }
}

static void teardown_impl() {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(_characters[i] != NULL) {
            free(_characters[i]);
        }
    }
    free(_characters);
}

static int contains_npc_impl(point_t* p) {
    int i;
    for(i = 0; i < _characters_count; i++) {
        if(p->distance(p, _characters[i]->position) == 0) {
            return i;
        }
    }
    return -1;
}

static char get_char_for_npc_at_index_impl(int i) {
    return characterAPI.char_for_npc_type(_characters[i]);
}


// Temp for player movement!
static void temp_setup_player_movement() {
    player_path = pathfinderAPI.construct(dungeonAPI.get_dungeon(), 0);
    point_t* rand_dest = dungeonAPI.rand_point(dungeonAPI.get_dungeon());
    characterAPI.get_pc()->destination = rand_dest;
    logger.i("Setting player path from (%2d, %2d) to (%2d, %2d)",
             characterAPI.get_pc()->position->x,
             characterAPI.get_pc()->position->y,
             rand_dest->x,
             rand_dest->y);
//    characterAPI.get_pc()->set_destination(characterAPI.get_pc(), rand_dest);
    dijkstraAPI.dijkstra(player_path, characterAPI.get_pc()->destination, characterAPI.get_pc()->position);
    eventQueueAPI.add_event(characterAPI.get_pc(), MOVE);
}

static int point_to_index(point_t* p) {
    // since outer rows and cols aren't being used
    // subtract one from both so the index starts at 0
    return ((p->y - 1) * (DUNGEON_WIDTH-2)) + (p->x - 1);
}

static void index_to_point(int index, point_t* p) {
    p->x = (index % (DUNGEON_WIDTH-2))+1;
    p->y = (index / (DUNGEON_WIDTH-2))+1;
}

void temp_handle_player_move() {
    character_t* pc = characterAPI.get_pc();
    vertex_t* v = player_path->vertices[point_to_index(pc->position)];
    point_t next;
    index_to_point(v->prev, &next);
    pc->set_position(pc, &next);
    eventQueueAPI.add_event(pc, MOVE);
}

int temp_is_finished() {
    character_t* pc = characterAPI.get_pc();
    return (pc->position->distance(pc->position, pc->destination) == 0);
}

character_store_namespace const characterStoreAPI = {
    setup_impl,
    teardown_impl,
    contains_npc_impl,
    get_char_for_npc_at_index_impl
};
