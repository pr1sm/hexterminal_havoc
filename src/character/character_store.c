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
#include "../dungeon/dungeon.h"
#include "../point/point.h"

static character_t** _characters = NULL;
static int _characters_size = 0;
static int _characters_count = 0;

static void setup_impl() {
    // TODO: change to get by CLI
    int nummon = 6;
    int i;
    dungeon_t* d = dungeonAPI.get_dungeon();
    _characters_size = nummon + 1; // num of monsters + pc
    _characters = (character_t**) calloc(_characters_size, sizeof(*_characters));
    _characters_count = _characters_size;
    _characters[0] = characterAPI.get_pc();
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

character_store_namespace const characterStoreAPI = {
    setup_impl,
    teardown_impl,
    contains_npc_impl,
    get_char_for_npc_at_index_impl
};
