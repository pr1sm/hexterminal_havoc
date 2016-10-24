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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern int CHARACTER_COUNT;

typedef struct character_store_namespace {
    void (*const setup)();
    void (*const teardown)();
    int  (*const contains_npc)(point_t* p);
    char (*const get_char_for_npc_at_index)(int i);
    character_t** (*const get_characters)();
    int  (*const is_finished)();
    void (*const npc_cleanup)();
    void (*const start_monster_list)();
    void (*const move_floors)();
} character_store_namespace;
extern character_store_namespace const characterStoreAPI;
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* character_store_h */
