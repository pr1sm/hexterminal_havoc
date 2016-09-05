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

#include "dungeon/dungeon.h"
#include "room/room.h"
#include "logger/logger.h"

int main(int argc, const char * argv[]) {
    
    logger.set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    
    dungeonAPI.construct();
    
    dungeonAPI.generate_terrain();
    dungeonAPI.place_rooms();
    
    dungeonAPI.destruct();
    
    return 0;
}
