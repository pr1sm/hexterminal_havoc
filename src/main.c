//
//  main.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/1/16.
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

int main(int argc, char * argv[]) {
    
    envAPI.setup_environment();
    envAPI.parse_args(argc, argv);
    
    if(DEBUG_MODE) {
        logger.set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    } else {
        logger.set_modes_enabled(LOG_I | LOG_W | LOG_E | LOG_F);
    }
    
    dungeon_t* d = dungeonAPI.get_dungeon();
    
    if(LOAD_DUNGEON) {
        d->load(d);
    } else {
        dungeonAPI.generate(d);
    }
    
    d->update_path_maps(d);
    
    characterStoreAPI.setup();
    
    if(DEBUG_MODE) {
        d->print(d, PM_ROOM_PATH_MAP);
        d->print(d, PM_TUNN_PATH_MAP);
    }
    
    d->print(d, PM_DUNGEON);
    
    if(SAVE_DUNGEON) {
        d->save(d);
    }
    int next_turn = 1;
    int win_status = characterStoreAPI.is_finished();
    while(!win_status && next_turn) {
        next_turn = eventQueueAPI.perform_event();
        win_status = characterStoreAPI.is_finished();
        d->print(d, PM_DUNGEON);
        usleep(250000);
    }
    
    if(win_status == 1) {
        printf("YOU LOSE!\n");
    } else {
        printf("BY SHEER LUCK, YOU WON!\n");
    }
    
    characterStoreAPI.teardown();
    
    dungeonAPI.destruct(d);
    envAPI.cleanup();
    
    return 0;
}
