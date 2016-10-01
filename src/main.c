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

#include "env/env.h"
#include "dungeon/dungeon.h"
#include "logger/logger.h"
#include "tile/tile.h"

int main(int argc, char * argv[]) {
    
    envAPI.setup_environment();
    envAPI.parse_args(argc, argv);
    
    if(DEBUG_MODE) {
        logger.set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    } else {
        logger.set_modes_enabled(LOG_I | LOG_W | LOG_E | LOG_F);
    }
    
    dungeon_t* d = dungeonAPI.construct();
    
    if(LOAD_DUNGEON) {
        d->load(d);
    } else {
        dungeonAPI.generate(d);
    }
    
    d->update_path_maps(d);
    
    d->print(d, PM_DUNGEON);
    
    d->print(d, PM_ROOM_PATH_MAP);
    
    d->print(d, PM_TUNN_PATH_MAP);
    
    if(SAVE_DUNGEON) {
        d->save(d);
    }
    
    dungeonAPI.destruct(d);
    envAPI.cleanup();
    
    return 0;
}
