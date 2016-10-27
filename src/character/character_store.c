//
//  character_store.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ncurses.h>

#include "character_store.h"
#ifdef __cplusplus
    #include "character.h"
#else
    #include "character_t.h"
#endif // __cplusplus
#include "ai.h"
#include "pc_control.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"
#include "../events/event_queue.h"
#include "../env/env.h"
#include "../dungeon/pathfinder.h"

static character_t** _characters = NULL;
static character_id_t* _alive_characters = NULL;
static int _characters_size = 0;
static int _characters_count = 0;

int CHARACTER_COUNT = 0;

static void setup_npc(character_t* npc);
static character_t* npc_for_id(character_id_t id);
static void set_destination(character_t* c, point_t* dest);

static void print_char(character_t* npc) {
    point_t* npc_pos;
    point_t* npc_dest;
    uint8_t  npc_attrs;
#ifdef __cplusplus
    npc_pos   = characterAPI.get_pos(npc);
    npc_dest  = characterAPI.get_dest(npc);
    npc_attrs = characterAPI.get_attrs(npc);
#else
    npc_pos   = npc->position;
    npc_dest  = npc->destination;
    npc_attrs = npc->attrs;
#endif // __cplusplus
    if(DEBUG_MODE && !NCURSES_MODE) {
        printf("NPC: spawn: (%2d, %2d) ",
               npc_pos->x,
               npc_dest->y);
        if(npc_dest != NULL) {
            printf("dest: (%2d, %2d) ",
                   npc_pos->x,
                   npc_dest->y);
        } else {
            printf("dest: NONE       ");
        }
        printf("attrs: %x\n", npc_attrs);
    }
    logger.d("NPC: spawn: (%2d, %2d) ",
           npc_pos->x,
           npc_pos->y);
    if(npc_dest != NULL) {
        logger.d("dest: (%2d, %2d) ",
               npc_dest->x,
               npc_dest->y);
    } else {
        logger.d("dest: NONE     ");
    }
    logger.d("attrs: %x\n", npc_attrs);
}

static void setup_impl() {
    int nummon = NUM_MONSTERS;
    int i;
    dungeon_t* d = dungeonAPI.get_dungeon();
    _characters_size = nummon; // num of monsters
    _characters = (character_t**) calloc(_characters_size, sizeof(*_characters));
    _characters_count = _characters_size;
    CHARACTER_COUNT = _characters_count;
    _alive_characters = (character_id_t*) calloc(_characters_count, sizeof(*_alive_characters));
    character_t* pc = characterAPI.get_pc();
    point_t* pc_pos;
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(pc);
#else
    pc_pos = pc->position;
#endif // __cplusplus
    if(PC_AI_MODE) {
        setup_pc_movement();
    } else {
        setup_control_movement();
    }
    for(i = 0; i < _characters_size; i++) {
        point_t* spawn = pointAPI.construct(0, 0);
        do {
            dungeonAPI.rand_point(d, spawn);
        } while(spawn->distance(spawn, pc_pos) <= 9); // have a radius of 3 blocks
        
        character_t* npc = characterAPI.construct_npc(spawn);
#ifdef __cplusplus
        characterAPI.set_id(npc, i);
#else
        npc->id = i;
#endif // __cplusplus
        
        setup_npc(npc);
        
        eventQueueAPI.add_event(npc);
        _characters[i] = npc;
        _alive_characters[i] = i;
        
        free(spawn);
        print_char(npc);
    }
}

static void teardown_impl() {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(_characters[i] != NULL) {
            characterAPI.destruct(_characters[i]);
        }
    }
    characterAPI.destruct(characterAPI.get_pc());
    free(_characters);
    free(_alive_characters);
    if(_PLAYER_PATH != NULL) {
        pathfinderAPI.destruct(_PLAYER_PATH);
    }
}

static int contains_npc_impl(point_t* p) {
    int i;
    point_t* npc_pos;
    point_t* pc_pos;
    uint8_t  npc_is_dead;
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(characterAPI.get_pc());
#else
    pc_pos = characterAPI.get_pc()->position;
#endif // __cplusplus
    // check npcs first, then player
    for(i = 0; i < _characters_count; i++) {
#ifdef __cplusplus
        npc_pos = characterAPI.get_pos(_characters[i]);
        npc_is_dead = characterAPI.get_is_dead(_characters[i]);
#else
        npc_pos = _characters[i]->position;
        npc_is_dead = _characters[i]->is_dead;
#endif // __cplusplus
        if(p->distance(p, npc_pos) == 0 && !npc_is_dead) {
            return i+1;
        }
    }
    if(p->distance(p, pc_pos) == 0) {
        return 0;
    }
    return -1;
}

static char get_char_for_npc_at_index_impl(int i) {
    if(i == 0) {
        return characterAPI.char_for_npc_type(characterAPI.get_pc());
    }
    return characterAPI.char_for_npc_type(_characters[i-1]);
}

static character_t** get_characters_impl() {
    return _characters;
}

static void setup_npc(character_t* npc) {
    point_t* pc_pos;
    point_t* npc_pos;
    uint8_t npc_attrs;
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(characterAPI.get_pc());
    npc_pos = characterAPI.get_pos(npc);
    npc_attrs = characterAPI.get_attrs(npc);
#else
    pc_pos = characterAPI.get_pc()->position;
    npc_pos = npc->position;
    npc_attrs = npc->attrs;
#endif // __cplusplus
    // telepathic npcs get the pc position
    if(npc_attrs & TELEP_VAL) {
        set_destination(npc, pc_pos);
    } else {
        // check los on other npcs
        path_node_t* los_path = los_to_pc(npc_pos);
        if(los_path != NULL) {
            set_destination(npc, pc_pos);
            graphAPI.destruct_path(los_path);
        }
    }
}

static int is_finished_impl() {
    int i;
    point_t* pc_pos;
    point_t* npc_pos;
    uint8_t  npc_is_dead;
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(characterAPI.get_pc());
#else
    pc_pos = characterAPI.get_pc()->position;
#endif // __cplusplus
    // only 1 character left (pc) so pc has won
    if(_characters_count == 0) {
        return 2;
    }
    // check all npcs that ARE NOT the pc for collision
    for(i = 0; i < _characters_size; i++) {
#ifdef __cplusplus
        npc_pos = characterAPI.get_pos(_characters[i]);
        npc_is_dead = characterAPI.get_is_dead(_characters[i]);
#else
        npc_pos = _characters[i]->position;
        npc_is_dead = _characters[i]->is_dead;
#endif // __cplusplus
        if(pc_pos->distance(pc_pos, npc_pos) == 0 && !npc_is_dead) {
            return 1;
        }
    }
    return 0;
}

static void npc_cleanup_impl() {
    int i;
    int j;
    int old_count = _characters_count;
    uint8_t npc_is_dead = 0;
    // check if NPCs are dead and shift others over
    for(i = 0; i < _characters_count; i++) {
        character_t* npc = npc_for_id(_alive_characters[i]);
#ifdef __cplusplus
        npc_is_dead = characterAPI.get_is_dead(npc);
#else
        npc_is_dead = npc->is_dead;
#endif // __cplusplus
        if(npc_is_dead) {
            // shift over other npcs
            for(j = i; j < _characters_count-1; j++) {
                _alive_characters[j] = _alive_characters[j+1];
            }
            _characters_count--;
        }
    }
    logger.d("NPC cleanup: %d ~> %d", old_count, _characters_count);
}

void start_monster_list_impl() {
    char** monster_list;
    int i;
    int xdiff;
    int ydiff;
    int print_start = 0;
    int print_end = _characters_count < print_start+20 ? _characters_count : print_start+20;
    int next_cmd = 0;
    point_t* pc_pos;
    point_t* npc_pos;
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(characterAPI.get_pc());
#else
    pc_pos = characterAPI.get_pc()->position;
#endif // __cplusplus
    monster_list = (char**) calloc(_characters_count, sizeof(*monster_list));
    for(i = 0; i < _characters_count; i++) {
        character_t* npc = npc_for_id(_alive_characters[i]);
        if(npc != NULL) {
#ifdef __cplusplus
            npc_pos = characterAPI.get_pos(npc);
#else
            npc_pos = npc->position;
#endif // __cplusplus
            monster_list[i] = (char*) calloc(30, sizeof(**monster_list));
            xdiff = pc_pos->x - npc_pos->x;
            ydiff = pc_pos->y - npc_pos->y;
            sprintf(monster_list[i], "%2d. %c, %2d %s and %2d %s",
                    i+1,
                    characterAPI.char_for_npc_type(npc),
                    abs(ydiff),
                    ydiff > 0 ? "north" : "south",
                    abs(xdiff),
                    xdiff > 0 ? "west" : "east");
        }
    }
    
    do {
        clear();
        mvprintw(1, 1, "Monster List (%d/%d)", print_end-print_start, _characters_count);
        for(i = 0; i < print_end - print_start; i++) {
            mvprintw(i+2, 1, monster_list[i+print_start]);
        }
        refresh();
        next_cmd = getch();
        if(print_start > 0 && next_cmd == PC_ML_SCRL_UP) {
            print_start--;
        } else if(print_start < _characters_count-20 && next_cmd == PC_ML_SCRL_DOWN) {
            print_start++;
        }
        print_end = _characters_count < print_start+20 ? _characters_count : print_start+20;
    } while(next_cmd != PC_ML_CLOSE);
    
    for(i = 0; i < _characters_count; i++) {
        free(monster_list[i]);
    }
    free(monster_list);
}

static void move_floors_impl() {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(_characters[i] != NULL) {
            characterAPI.destruct(_characters[i]);
        }
    }
    free(_characters);
    free(_alive_characters);
    if(_PLAYER_PATH != NULL) {
        pathfinderAPI.destruct(_PLAYER_PATH);
    }
    
    characterStoreAPI.setup();
}

static character_t* npc_for_id(character_id_t id) {
    int i;
    uint8_t npc_id;
    for(i = 0; i < _characters_size; i++) {
#ifdef __cplusplus
        npc_id = characterAPI.get_id(_characters[i]);
#else
        npc_id = _characters[i]->id;
#endif // __cplusplus
        if(id == npc_id) {
            return _characters[i];
        }
    }
    return NULL;
}

static void set_destination(character_t* c, point_t* dest) {
#ifdef __cplusplus
    characterAPI.set_dest(c, dest);
#else
    c->set_destination(c, dest);
#endif // __cplusplus
}

character_store_namespace const characterStoreAPI = {
    setup_impl,
    teardown_impl,
    contains_npc_impl,
    get_char_for_npc_at_index_impl,
    get_characters_impl,
    is_finished_impl,
    npc_cleanup_impl,
    start_monster_list_impl,
    move_floors_impl
};
