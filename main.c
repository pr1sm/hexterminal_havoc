//
//  main.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dungeon/dungeon.h"

int main(int argc, const char * argv[]) {
    
    dungeonAPI.construct();
    
    dungeonAPI.generate_terrain();
    
    dungeonAPI.destruct();
    
    return 0;
}
