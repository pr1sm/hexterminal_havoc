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

#include "dungeon/dungeon.h"
#include "room/room.h"
#include "logger/logger.h" 

#include "heap/heap.h"
#include "tile/tile.h"
#include "point/point.h"
#include "dungeon/dijkstra.h"

void print_dungeon() {
    int i;
    int j;
    for(i = 0; i < 21; i++) {
        for(j = 0; j < 80; j++) {
            tile_t* t = _dungeon_array[i][j];
            char out = t->content == tc_BORDER ? '%' :
            t->content == tc_ROCK ? ' ' :
            t->content == tc_ROOM ? '.' :
            t->content == tc_PATH ? '#' : '$';
            printf("%c", out);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, const char * argv[]) {
    
    logger.set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    
    dungeonAPI.construct();
    
    dungeonAPI.generate_terrain();
    dungeonAPI.place_rooms();
    
    graph_t* g = dijkstraAPI.construct();
    int i;
    for(i = 0; i < _room_size - 1; i++) {
        dijkstraAPI.dijkstra(g, _room_array[i]->location, _room_array[i+1]->location);
        dijkstraAPI.place_path(g, _room_array[i+1]->location);
    }
    
    print_dungeon();
    
    dijkstraAPI.destruct(g);
    dungeonAPI.destruct();
    
    return 0;
}
