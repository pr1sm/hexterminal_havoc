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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void setup_control_movement() {
    character_t* pc;
    pc = characterAPI.get_pc();
    eventQueueAPI.add_event(pc);
}

void handle_control_move() {
    character_t** characters = characterStoreAPI.get_characters();
    dungeon_t* d = dungeonAPI.get_dungeon();
    character_t* pc;
    point_t* pc_pos;
    point_t* npc_pos;
    pc = characterAPI.get_pc();
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(pc);
#else
    pc_pos = pc->position;
#endif // __cplusplus
    pc_move_t move = mv_NONE;
    point_t* dest = pointAPI.construct(pc_pos->x, pc_pos->y);
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
            characterStoreAPI.start_monster_list();
            dungeonAPI.get_dungeon()->print(dungeonAPI.get_dungeon(), PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        }
    } while(!is_valid);
    
    if(move == mv_ML) {
        logger.i("Starting monster list...");
        characterStoreAPI.start_monster_list();
        logger.i("Ending monster list");
    } else if(move == mv_UPSTR) {
        logger.i("Moving upstairs");
        STAIR_FLAG = 1; // move upstairs
    } else if(move == mv_DNSTR) {
        logger.i("Moving downstairs");
        STAIR_FLAG = 2; // move downstairs
    } else if(move != mv_RS) {
        logger.i("Moving to point (%2d, %2d)", dest->x, dest->y);
        // move pc
#ifdef __cplusplus
        characterAPI.set_pos(pc, dest);
        pc_pos = characterAPI.get_pos(pc);
#else
        pc->set_position(pc, dest);
        pc_pos = pc->position;
#endif // __cplusplus
        
        // check for collision
        for(i = 0; i < CHARACTER_COUNT; i++) {
#ifdef __cplusplus
            npc_pos = characterAPI.get_pos(characters[i]);
#else
            npc_pos = characters[i]->position;
#endif // __cplusplus
            if(pc_pos->distance(pc_pos, npc_pos) == 0) {
#ifdef __cplusplus
                characterAPI.set_is_dead(characters[i], 1);
#else
                characters[i]->is_dead = 1;
#endif // __cplusplus
            }
        }
    } else if(move == mv_RS) {
        logger.i("Resting 1 turn");
    }
    
    pointAPI.destruct(dest);
    eventQueueAPI.add_event(pc);
}
    
#ifdef __cplusplus
}
#endif // __cplusplus

