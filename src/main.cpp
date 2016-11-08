//
//  main.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

#include "env/env.h"
#include "dungeon/dungeon.h"
#include "logger/logger.h"
#include "tile/tile.h"
#include "character/character_store.h"
#include "events/event_queue.h"
#include "parser/parser.h"

int main(int argc, char * argv[]) {
 
#ifdef DEBUG // Xcode terminal debugging with ncurses
    int argi;
    for (argi = 1; argi < argc; argi++)
    {
        if (strcmp(argv[argi], "--debug-in-terminal") == 0)
        {
            printf("Debugging in terminal enabled\n");
            getchar(); // Without this call debugging will be skipped
            break;
        }
    }
#endif // DEBUG
    
    env_constants::PARSE_MODE = 1; // parse only
    
    env::parse_args(argc, argv);
    env::setup_environment();
    
    if(env_constants::DEBUG_MODE) {
        logger::set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    } else {
        logger::set_modes_enabled(LOG_I | LOG_W | LOG_E | LOG_F);
    }
    
    parser p;
    p.parse_monsters();
    p.parse_items();
    
    if(env_constants::PARSE_MODE) {
        
        p.print_monsters();
        p.print_items();
        
        return 0;
    }
    
    dungeon* d = dungeon::get_dungeon();
    
    if(env_constants::LOAD_DUNGEON) {
        d->load();
    } else {
        d->generate();
    }
    
    d->update_path_maps();
    
    character_store::setup();
    
    if(env_constants::DEBUG_MODE) {
        d->print(PM_ROOM_PATH_MAP);
        d->print(PM_TUNN_PATH_MAP);
    }
    
    d->print(PM_DUNGEON);
    
    int next_turn = 1;
    int win_status = character_store::is_finished();
    while(!win_status && next_turn) {
        next_turn = event_queue::perform_event();
        if(env_constants::STAIR_FLAG == 1 || env_constants::STAIR_FLAG == 2) {
            env::move_floors();
            d = dungeon::get_dungeon();
        }
        win_status = character_store::is_finished();
        d->print(PM_DUNGEON);
        usleep(100000);
    }
    
    if(env_constants::SAVE_DUNGEON) {
        d->save();
    }
    
    env::cleanup();
    
    if(win_status == 1) {
        printf("YOU LOSE!\n");
    } else if(win_status == 2) {
        printf("BY SHEER LUCK, YOU WON!\n");
    } else {
        printf("YOU QUIT\n");
    }
    
    return 0;
}
