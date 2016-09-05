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

int main(int argc, const char * argv[]) {
    
    dungeonAPI.construct();
    
    dungeonAPI.generate_terrain();
    dungeonAPI.place_rooms();
    
    dungeonAPI.destruct();
    
    return 0;
}
