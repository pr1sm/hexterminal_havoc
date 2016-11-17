//
//  pc_control.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <ncurses.h>

#include "pc_control.h"
#include "character_store.h"
#include "../events/event_queue.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../items/item_store.h"
#include "../items/item.h"

void pc_control::setup_control_movement() {
    event_queue::add_event(character::get_pc());
}

void pc_control::handle_control_move() {
    character** characters = character_store::get_characters();
    dungeon* d = dungeon::get_dungeon();
    character* pc;
    point* pc_pos;
    point* npc_pos;
    pc = character::get_pc();
    pc_pos = pc->position;
    pc_move_t move = mv_NONE;
    point* dest = new point(pc_pos);
    int i;
    mvprintw(0, 0, "ENTER COMMAND:                                ");
    refresh();
    int is_valid = 0;
    do {
        move = mv_NONE;
        dest->x = pc_pos->x;
        dest->y = pc_pos->y;
        int ch = getch();
        switch (ch) {
            case PC_QUIT:
                env_constants::QUIT_FLAG = 1;
                is_valid = 1;
                break;
                
            case PC_UP_8:
            case PC_UP_k:
                move = mv_UP;
                break;
                
            case PC_UP_LEFT_7:
            case PC_UP_LEFT_y:
                move = mv_UL;
                break;
                
            case PC_UP_RIGHT_9:
            case PC_UP_RIGHT_u:
                move = mv_UR;
                break;
                
            case PC_LEFT_4:
            case PC_LEFT_h:
                move = mv_LT;
                break;
                
            case PC_RIGHT_6:
            case PC_RIGHT_l:
                move = mv_RT;
                break;
                
            case PC_DOWN_LEFT_1:
            case PC_DOWN_LEFT_b:
                move = mv_DL;
                break;
                
            case PC_DOWN_2:
            case PC_DOWN_j:
                move = mv_DN;
                break;
                
            case PC_DOWN_RIGHT_3:
            case PC_DOWN_RIGHT_n:
                move = mv_DR;
                break;
                
            case PC_REST_5:
            case PC_REST_SPACE:
                move = mv_RS;
                break;
                
            case PC_MONSTER_LIST:
                move = mv_ML;
                is_valid = 1;
                break;
                
            case PC_DOWNSTAIRS:
                move = mv_DNSTR;
                break;
                
            case PC_UPSTAIRS:
                move = mv_UPSTR;
                break;
                
            case PC_INV_LIST:
                move = mv_IL;
                is_valid = 1;
                break;
                
            default:
                mvprintw(0, 0, "INVALID COMMAND: %3d                            ", ch);
                refresh();
                break;
        }
        
        if(!is_valid && move == mv_UPSTR) {
            if(d->tiles[pc_pos->y][pc_pos->x]->content == tc_UPSTR) {
                is_valid = 1;
            }
            
            if(!is_valid) {
                mvprintw(0, 0, "Whoops! You can't go up without stairs!");
            }
        } else if(!is_valid && move == mv_DNSTR) {
            if(d->tiles[pc_pos->y][pc_pos->x]->content == tc_DNSTR) {
                is_valid = 1;
            }
            
            if(!is_valid) {
                mvprintw(0, 0, "Whoops! You can't go down without stairs!");
            }
        } else if(!is_valid && move != mv_NONE) {
            if(move == mv_UL || move == mv_UP || move == mv_UR) {
                dest->y -= 1;
            } else if(move == mv_DL || move == mv_DN || move == mv_DR) {
                dest->y += 1;
            }
            if(move == mv_UL || move == mv_LT || move == mv_DL) {
                dest->x -= 1;
            } else if(move == mv_UR || move == mv_RT || move == mv_DR) {
                dest->x += 1;
            }
            
            if(dest->x < 1 || dest->x > DUNGEON_WIDTH-2 || dest->y < 1 || dest->y > DUNGEON_HEIGHT-2) {
                is_valid = 0;
            } else if(d->tiles[dest->y][dest->x]->content != tc_ROCK) {
                is_valid = 1;
            }
            
            if(!is_valid) {
                mvprintw(0, 0, "Whoops! You can't move there, try again");
                refresh();
            }
        }
        if(move == mv_ML) {
            character_store::start_monster_list();
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_IL) {
            pc_control::show_inventory();
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        }
    } while(!is_valid);
    
    if(move == mv_ML) {
        logger::i("Starting monster list...");
        character_store::start_monster_list();
        logger::i("Ending monster list");
    } else if(move == mv_UPSTR) {
        logger::i("Moving upstairs");
        env_constants::STAIR_FLAG = 1; // move upstairs
    } else if(move == mv_DNSTR) {
        logger::i("Moving downstairs");
        env_constants::STAIR_FLAG = 2; // move downstairs
    } else if(move != mv_RS) {
        logger::i("Moving to point (%2d, %2d)", dest->x, dest->y);
        // move pc
        pc->set_position(dest);
        pc_pos = pc->position;
        
        // check for collision
        for(i = 0; i < character_store::CHARACTER_COUNT; i++) {
            npc_pos = characters[i]->position;
            if(pc_pos->distance_to(npc_pos) == 0) {
                characters[i]->is_dead = 1;
            }
            characters[i]->is_seen = (pc_pos->distance_to(npc_pos) <= 3);
        }
    } else if(move == mv_RS) {
        logger::i("Resting 1 turn");
    }
    
    // check items
    if(item_store::contains_item(pc->position)) {
        item_store::pickup_item(pc->position);
    }
    
    delete dest;
    event_queue::add_event(pc);
}

void pc_control::show_inventory() {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    character* pc = character::get_pc();
    int i;
    clear();
    mvprintw(0, 0, "Press ESC to close...");
    if(pc->inventory_size > 0) {
        mvprintw(1, 1, "Inventory List");
        for(i = 0; i < pc->inventory_size; i++) {
            item* item = pc->inventory[i];
            if(item == NULL) {
                mvprintw(i+2, 3, "%d) EMPTY", i);
            } else if(item->state == is_picked_up) {
                attron(COLOR_PAIR(item->color));
                mvprintw(i+2, 3, "%d) %c - %s (sp: %d, dmg: %s)", i, item->symb, item->name.c_str(), item->speed, item->damage->to_string().c_str());
                attroff(COLOR_PAIR(item->color));
            } else {
                mvprintw(i+2, 3, "%d) EMPTY", i);
            }
        }
    } else {
       mvprintw(1, 1, "Inventory List - EMPTY! (pickup some items around the dungeon!)");
    }
    refresh();
    int next_cmd = 0;
    do {
        next_cmd = getch();
    } while(next_cmd != PC_ML_CLOSE);
}

void pc_control::show_equipment() {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    character* pc = character::get_pc();
    int i;
    clear();
    mvprintw(0, 0, "Press ESC to close...");
    if(pc->inventory_size > 0) {
        mvprintw(1, 1, "Inventory List");
        for(i = 0; i < pc->inventory_size; i++) {
            item* item = pc->inventory[i];
            if(item == NULL) {
                mvprintw(i+2, 3, "%d) EMPTY", i);
            } else if(item->state == is_picked_up) {
                attron(COLOR_PAIR(item->color));
                mvprintw(i+2, 3, "%d) %c - %s (sp: %d, dmg: %s)", i, item->symb, item->name.c_str(), item->speed, item->damage->to_string().c_str());
                attroff(COLOR_PAIR(item->color));
            } else {
                mvprintw(i+2, 3, "%d) EMPTY", i);
            }
        }
    } else {
        mvprintw(1, 1, "Inventory List - EMPTY! (pickup some items around the dungeon!)");
    }
    refresh();
    int next_cmd = 0;
    do {
        next_cmd = getch();
    } while(next_cmd != PC_ML_CLOSE);
}

void pc_control::inspect_inventory() {
    
}