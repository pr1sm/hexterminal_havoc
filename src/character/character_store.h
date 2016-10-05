//
//  character_store.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef character_store_h
#define character_store_h

#include "character.h"
#include "../point/point.h"

// Temp for player movement!
void temp_handle_player_move();
int  temp_is_finished();

typedef struct character_store_namespace {
    void (*const setup)();
    void (*const teardown)();
    int  (*const contains_npc)(point_t* p);
    char (*const get_char_for_npc_at_index)(int i);
} character_store_namespace;
extern character_store_namespace const characterStoreAPI;

#endif /* character_store_h */
