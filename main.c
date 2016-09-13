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

int main(int argc, char * argv[]) {
    
    parse_args(argc, argv);
    
    setup_environment();
    
    if(DEBUG_MODE) {
        logger.set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    } else {
        logger.set_modes_enabled(LOG_I | LOG_W | LOG_E | LOG_F);
    }
    
    dungeonAPI.construct();
    
    dungeonAPI.generate_terrain();
    dungeonAPI.place_rooms();
    dungeonAPI.pathfind();
    dungeonAPI.print();
    
    dungeonAPI.destruct();
    
    return 0;
}
