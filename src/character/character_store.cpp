//
//  character_store.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ncurses.h>


#include "character_store.h"
#include "character.h"
#include "ai.h"
#include "pc_control.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"
#include "../events/event_queue.h"
#include "../env/env.h"
#include "../dungeon/pathfinder.h"
#include "../parser/parser.h"
#include "../parser/monster_description.h"

int character_store::CHARACTER_COUNT = 0;
int character_store::_characters_size = 0;
int character_store::_characters_count = 0;
character_id_t* character_store::_alive_characters = NULL;
character** character_store::_characters = NULL;

void character_store::print_char(character* npc) {
    point* npc_pos;
    point* npc_dest;
    uint8_t  npc_attrs;
    npc_pos   = npc->position;
    npc_dest  = npc->destination;
    npc_attrs = npc->attrs;
    if(env_constants::DEBUG_MODE && !env_constants::NCURSES_MODE) {
        printf("NPC: spawn: (%2d, %2d) ",
               npc_pos->x,
               npc_pos->y);
        if(npc_dest != NULL) {
            printf("dest: (%2d, %2d) ",
                   npc_dest->x,
                   npc_dest->y);
        } else {
            printf("dest: NONE       ");
        }
        printf("attrs: %x\n", npc_attrs);
    }
    logger::d("NPC: spawn: (%2d, %2d) ",
           npc_pos->x,
           npc_pos->y);
    if(npc_dest != NULL) {
        logger::d("dest: (%2d, %2d) ",
               npc_dest->x,
               npc_dest->y);
    } else {
        logger::d("dest: NONE     ");
    }
    logger::d("attrs: %x\n", npc_attrs);
}

void character_store::setup() {
    int nummon = env_constants::NUM_MONSTERS;
    int i;
    _characters_size = nummon; // num of monsters
    _characters = (character**) calloc(_characters_size, sizeof(*_characters));
    _characters_count = _characters_size;
    CHARACTER_COUNT = _characters_count;
    _alive_characters = (character_id_t*) calloc(_characters_count, sizeof(*_alive_characters));
    if(env_constants::PC_AI_MODE) {
        ai::setup_pc_movement();
    } else {
        pc_control::setup_control_movement();
    }
    for(i = 0; i < _characters_size; i++) {
        
        character* npc = gen_npc(env_constants::USE_MPARSE);
        npc->id = i+1;
        
        setup_npc(npc);
        
        event_queue::add_event(npc);
        _characters[i] = npc;
        _alive_characters[i] = i+1;
        
        print_char(npc);
    }
}

void character_store::teardown() {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(_characters[i] != NULL) {
            delete _characters[i];
        }
    }
    character::teardown_pc();
    free(_characters);
    free(_alive_characters);
    if(ai::PLAYER_PATH != NULL) {
        pathfinder::destruct(ai::PLAYER_PATH);
    }
}

int character_store::contains_npc(point* p) {
    int i;
    point* npc_pos;
    point* pc_pos;
    uint8_t  npc_is_dead;
    pc_pos = character::get_pc()->position;
    // check npcs first, then player
    for(i = 0; i < _characters_count; i++) {
        npc_pos = _characters[i]->position;
        npc_is_dead = _characters[i]->is_dead;
        if(p->distance_to(npc_pos) == 0 && !npc_is_dead) {
            return i+1;
        }
    }
    if(p->distance_to(pc_pos) == 0) {
        return 0;
    }
    return -1;
}

char character_store::get_char_for_npc_at_index(int i, int mode) {
    if(i == 0) {
        return character::get_pc()->get_print_symb(mode);
    }
    return _characters[i-1]->get_print_symb(mode);
}

character** character_store::get_characters() {
    return _characters;
}

character_id_t* character_store::get_alive_characters() {
    return _alive_characters;
}

void character_store::setup_npc(character* npc) {
    point* pc_pos;
    point* npc_pos;
    uint8_t npc_attrs;
    pc_pos = character::get_pc()->position;
    npc_pos = npc->position;
    npc_attrs = npc->attrs;
    // telepathic npcs get the pc position
    if(npc_attrs & TELEP_VAL) {
        npc->set_destination(pc_pos);
    } else {
        // check los on other npcs
        path_node* los_path = ai::los_to_pc(npc_pos);
        if(los_path != NULL) {
            npc->set_destination(pc_pos);
            delete los_path;
        }
    }
}

int character_store::is_finished() {
    character* pc = character::get_pc();
    // only 1 character left (pc) so pc has won
    if(_characters_count == 0) {
        return 2;
    }
    // check pc health
    if(pc->hitpoints <= 0) {
        return 1; 
    }
    
    return 0;
}

void character_store::npc_cleanup() {
    int i;
    int j;
    int old_count = _characters_count;
    uint8_t npc_is_dead = 0;
    // check if NPCs are dead and shift others over
    for(i = 0; i < _characters_count; i++) {
        npc_is_dead = 0;
        character* npc = npc_for_id(_alive_characters[i]);
        npc_is_dead = npc->is_dead;
        if(npc_is_dead) {
            logger::d("npc %d is dead, shifting over", _alive_characters[i]);
            // shift over other npcs
            for(j = i; j < _characters_count-1; j++) {
                _alive_characters[j] = _alive_characters[j+1];
            }
            _characters_count--;
        }
    }
    CHARACTER_COUNT = _characters_count;
    logger::d("NPC cleanup: %d ~> %d", old_count, _characters_count);
}

void character_store::start_monster_list() {
    char** monster_list;
    int i;
    int xdiff;
    int ydiff;
    int print_start = 0;
    int print_end = _characters_count < print_start+20 ? _characters_count : print_start+20;
    int next_cmd = 0;
    point* pc_pos;
    point* npc_pos;
    
    if(!env_constants::NCURSES_MODE) {
        return; // this can only be shown when using pc control, which requires NCURSES_MODE to be active.
    }
    
    pc_pos = character::get_pc()->position;
    monster_list = (char**) calloc(_characters_count, sizeof(*monster_list));
    for(i = 0; i < _characters_count; i++) {
        character* npc = npc_for_id(_alive_characters[i]);
        if(npc != NULL) {
            npc_pos = npc->position;
            monster_list[i] = (char*) calloc(30, sizeof(**monster_list));
            xdiff = pc_pos->x - npc_pos->x;
            ydiff = pc_pos->y - npc_pos->y;
            sprintf(monster_list[i], "%2d. %c, %2d %s and %2d %s",
                    i+1,
                    npc->symb,
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

void character_store::move_floors() {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(_characters[i] != NULL) {
            delete _characters[i];
        }
    }
    free(_characters);
    free(_alive_characters);
    if(ai::PLAYER_PATH != NULL) {
        pathfinder::destruct(ai::PLAYER_PATH);
        ai::PLAYER_PATH = NULL;
    }
    
    character_store::setup();
}

void character_store::update_position(character* c) {
    int i, j;
    character* npc = NULL;
    for(i = 0; i < _characters_count; i++) {
        npc = npc_for_id(_alive_characters[i]);
        if(npc == c) {
            continue;
        }
        if(npc->position->distance_to(c->position) == 0) { // c and npc's position are the same
            int offsets[] = {-1, -1, -1,  0,  1,  1,  1,  0, -1, -1, -1,  0,  1,  1,  1,  0, -1};
            int x_start = rand() % 8;
            int y_start = rand() % 8;
            dungeon* d = dungeon::get_dungeon();
            for(j = 0; j < 8; j++) {
                int new_x = offsets[x_start + j + 2] + c->position->x;
                int new_y = offsets[y_start + j] + c->position->y;
                point new_p(new_x, new_y);
                if(new_x < 1 || new_x > DUNGEON_WIDTH-2 || new_y < 1 || new_y > DUNGEON_HEIGHT-2) {
                    continue; // out of bounds
                }
                tile* t = d->tiles[new_y][new_x];
                if(t->content != tc_ROCK && t->content != tc_BORDER && (contains_npc(&new_p) == -1)) {
                    npc->set_position(&new_p);
                    break;
                }
            }
        }
    }
}

character* character_store::npc_for_id(character_id_t id) {
    int i;
    uint8_t npc_id;
    for(i = 0; i < _characters_size; i++) {
        npc_id = _characters[i]->id;
        if(id == npc_id) {
            return _characters[i];
        }
    }
    return NULL;
}

character* character_store::gen_npc(bool use_descriptor) {
    character* pc = character::get_pc();
    point* pc_pos = pc->position;
    dungeon* d = dungeon::get_dungeon();
    point spawn(0, 0);
    do {
        dungeon::rand_point(d, &spawn);
    } while(spawn.distance_to(pc_pos) <= 3); // have a radius of 3 blocks
    
    monster_description** monster_list = parser::get_parser()->monster_list;
    
    if(!use_descriptor || monster_list == NULL) {
        return new character(NPC, &spawn);
    }
    
    int rand_idx = rand() % parser::get_parser()->monster_len;
    monster_description* descriptor = monster_list[rand_idx];
    
    return new character(NPC, &spawn, descriptor);
}
