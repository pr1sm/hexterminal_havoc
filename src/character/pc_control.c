//
//  pc_control.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/19/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <ncurses.h>

#include "pc_control.h"
#include "character_store.h"
#include "../events/event_queue.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"

void setup_control_movement() {
    eventQueueAPI.add_event(characterAPI.get_pc());
}

void handle_control_move() {
    character_t** characters = characterStoreAPI.get_characters();
    dungeon_t* d = dungeonAPI.get_dungeon();
    character_t* pc = characterAPI.get_pc();
    pc_move_t move = mv_NONE;
    point_t* dest = pointAPI.construct(pc->position->x, pc->position->y);
    int i;
    mvprintw(0, 0, "ENTER COMMAND:                          ");
    refresh();
    int is_valid = 0;
    do {
        dest->x = pc->position->x;
        dest->y = pc->position->y;
        int ch = getch();
        switch (ch) {
            case PC_QUIT:
                QUIT_FLAG = 1;
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
                
            default:
                mvprintw(0, 0, "INVALID COMMAND: %3d                     ", ch);
                refresh();
                break;
        }
        
        if(!is_valid && move != NONE) {
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
                mvprintw(0, 0, "Whoops! You can't move there, try again", ch);
                refresh();
            }
        }
    } while(!is_valid);
    
    if(move != mv_RS) {
        // move pc
        pc->position->x = dest->x;
        pc->position->y = dest->y;
        
        // check for collision
        for(i = 0; i < CHARACTER_COUNT; i++) {
            if(pc->position->distance(pc->position, characters[i]->position) == 0) {
                characters[i]->is_dead = 1;
            }
        }
    }
    
    pointAPI.destruct(dest);
    eventQueueAPI.add_event(pc);
}
