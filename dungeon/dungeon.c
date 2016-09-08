//
//  dungeon.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dungeon.h"
#include "dijkstra.h"
#include "../tile/tile.h"
#include "../room/room.h"
#include "../logger/logger.h"
#include "../env_flags/env_flags.h"

#define POINT_LIMIT (DUNGEON_HEIGHT*DUNGEON_WIDTH/25)
#define ROCK_MAX 255
#define ROCK_HARD 165
#define ROCK_MED  110
#define ROCK_SOFT 55

// Array of tiles for the dungeon
// size will be 21 rows x 80 cols
tile_t*** _dungeon_array;

static room_t** _room_array;
static int _room_size;
static void accent_dungeon();
static void diffuse_dungeon();
static void smooth_dungeon();
static void border_dungeon();
static int is_open_space();
static void add_rooms();
static void write_dungeon_pgm(const char* file_name, int zone);

static void d_log_room(room_t* r) {
    if(DEBUG_MODE) {
        printf("Room: x: %2d, y: %2d, w: %2d, h: %2d\n", r->location->x, r->location->y, r->width, r->height);
    }
    logger.d("Room: x: %2d, y: %2d, w: %2d, h: %2d", r->location->x, r->location->y, r->width, r->height);
}

void dungeon_construct() {
    logger.i("Constructing Dungeon...");
    int i, j;
    srand((unsigned)time(NULL));
    
    _dungeon_array = calloc(DUNGEON_HEIGHT, sizeof(*_dungeon_array));
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        _dungeon_array[i] = calloc(DUNGEON_WIDTH, sizeof(**_dungeon_array));
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            _dungeon_array[i][j] = tileAPI.construct(j, i);
        }
    }
    logger.i("Dungeon Constructed");
}

void dungeon_destruct() {
    logger.i("Destructing Dungeon...");
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tileAPI.destruct(_dungeon_array[i][j]);
        }
        free(_dungeon_array[i]);
    }
    free(_dungeon_array);
    logger.i("Dungeon Destructed");
}

void dungeon_generate_terrain() {
    logger.i("Generating Terrain...");
    accent_dungeon();
    diffuse_dungeon();
    smooth_dungeon();
    border_dungeon();
    logger.i("Terrain Generated");
    
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            if(_dungeon_array[i][j]->content == tc_UNSET) {
                logger.f("Unset Tile @ (%2d, %2d) after terrain generation!", j, i);
                exit(2);
            }
        }
    }
}

void dungeon_place_rooms() {
    logger.i("Placing rooms in dungeon...");
    int i, j;
    int x, y, width, height, room_valid = 1;
    int overlap_valid = 1;
    int place_attempts_fail = 0;
    _room_size = 6;
    _room_array = calloc(_room_size, sizeof(*_room_array));
    
    // initally create the 6 rooms and check if overlap is valid.
    logger.d("Generating initial rooms...");
    do {
        for(i = 0; i < _room_size; i++) {
            do {
                room_valid = 1;
                x = (rand() % (DUNGEON_WIDTH - 2)) + 1;
                y = (rand() % (DUNGEON_HEIGHT - 2)) + 1;
                width = (rand() % 8) + 4;
                height = (rand() % 8) + 3;
                if(((x + width) > DUNGEON_WIDTH - 2) ||
                   ((y + height) > DUNGEON_HEIGHT - 2)) {
                    room_valid = 0;
                }
            } while(!room_valid);
            _room_array[i] = roomAPI.construct(x, y, width, height);
        }
        
        overlap_valid = 1;
        for(i = 0; i < _room_size; i++) {
            for(j = 0; j < i; j++) {
                if(roomAPI.is_overlap(_room_array[i], _room_array[j])) {
                    overlap_valid = 0;
                    break;
                }
            }
            if(!overlap_valid) {
                break;
            }
        }
    } while(!overlap_valid);
    logger.d("Initial Rooms Generated");
    
    // Check if we have open space and add rooms one at a time
    logger.d("Checking on adding more rooms");
    while(is_open_space() && place_attempts_fail < 2000) {
        logger.d("Adding one more room");
        room_valid = 1;
        do {
            x = (rand() % (DUNGEON_WIDTH - 2)) + 1;
            y = (rand() % (DUNGEON_HEIGHT - 2)) + 1;
            width = (rand() % 8) + 4;
            height = (rand() % 8) + 3;
            if(((x + width) > DUNGEON_WIDTH - 2) ||
               ((y + height) > DUNGEON_HEIGHT - 2)) {
                room_valid = 0;
                place_attempts_fail++;
            } else {
                room_valid = 1;
            }
        } while(!room_valid && place_attempts_fail < 2000);
        if(place_attempts_fail >= 2000) {
            break;
        }
        
        room_t* new_room = roomAPI.construct(x, y, width, height);
        
        logger.d("Room defined: (%2d, %2d, %2d, %2d), checking overlap",
                 new_room->location->x,
                 new_room->location->y,
                 new_room->width,
                 new_room->height);
        overlap_valid = 1;
        for(i = 0; i < _room_size; i++) {
            if(roomAPI.is_overlap(new_room, _room_array[i])) {
                overlap_valid = 0;
                break;
            }
        }
        if(!overlap_valid) {
            place_attempts_fail++;
            roomAPI.destruct(new_room);
            logger.d("Overlap Detected, Removing room and trying again");
        } else {
            logger.t("Room is valid, reallocating room array and adding room...");
            room_t** new_room_array = (room_t**) realloc(_room_array, (_room_size + 1) * sizeof(*_room_array));
            if(new_room_array == NULL) {
                logger.e("Room Array Reallocation failed!");
                break;
            }
            _room_array = new_room_array;
            _room_array[_room_size++] = new_room;
            logger.t("Room added");
        }
    }
    
    if(place_attempts_fail >= 2000) {
        logger.d("Stopping room placement due to attempt failures: %d", place_attempts_fail);
    }
    
    add_rooms();
    
    for(i = 0; i < _room_size; i++) {
        d_log_room(_room_array[i]);
    }
    
    logger.i("Rooms Placed in Dungeon");
}

void dungeon_pathfind() {
    logger.i("Generating Corridors...");
    
    graph_t* g = dijkstraAPI.construct(0);
    point_t src;
    point_t dest;
    int i;
    for(i = 0; i < _room_size - 1; i++) {
        src.x = (rand() % _room_array[i]->width) + _room_array[i]->location->x;
        src.y = (rand() % _room_array[i]->height) + _room_array[i]->location->y;
        dest.x = (rand() % _room_array[i+1]->width) + _room_array[i+1]->location->x;
        dest.y = (rand() % _room_array[i+1]->height) + _room_array[i+1]->location->y;
        logger.d("Room Path %2d: Routing from (%2d, %2d) to (%2d, %2d)", i, src.x, src.y, dest.x, dest.y);
        dijkstraAPI.dijkstra(g, &src, &dest);
        dijkstraAPI.place_path(g, &dest);
    }
    dijkstraAPI.destruct(g);
    
    // Connect last room to first room
    g = dijkstraAPI.construct(1);
    
    src.x = (rand() % _room_array[_room_size - 1]->width) + _room_array[_room_size - 1]->location->x;
    src.y = (rand() % _room_array[_room_size - 1]->height) + _room_array[_room_size - 1]->location->y;
    dest.x = (rand() % _room_array[0]->width) + _room_array[0]->location->x;
    dest.y = (rand() % _room_array[0]->height) + _room_array[0]->location->y;
    logger.d("Room Path %2d: Routing from (%2d, %2d) to (%2d, %2d)", i, src.x, src.y, dest.x, dest.y);
    dijkstraAPI.dijkstra(g, &src, &dest);
    dijkstraAPI.place_path(g, &dest);
    
    dijkstraAPI.destruct(g);
    
    logger.i("Corridors Generated");
}

void dungeon_print() {
    logger.i("Printing Dungeon...");
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            char c = tileAPI.char_for_content(_dungeon_array[i][j]);
            if(c == '?') {
                logger.e("Bad Tile Found @ (%2d, %2d) with content: %d", _dungeon_array[i][j]->location->x, _dungeon_array[i][j]->location->y, _dungeon_array[i][j]->content);
            }
            printf("%c", tileAPI.char_for_content(_dungeon_array[i][j]));
        }
        printf("\n");
    }
    printf("\n");
    logger.i("Dungeon Printed");
}

static void write_dungeon_pgm(const char* file_name, int zone) {
    int i, j;
    FILE* pgm = fopen(file_name, "w+");
    
    fprintf(pgm, "P2 \r\n%d %d \r\n255 \r\n", DUNGEON_WIDTH, DUNGEON_HEIGHT);
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            int hardness = _dungeon_array[i][j]->rock_hardness;
            int grey_val = hardness == ROCK_HARD ? 255 :
            hardness == ROCK_MED ? 170 :
            hardness == ROCK_SOFT ? 80 : 0;
            grey_val = zone ? grey_val : hardness;
            fprintf(pgm, "%d ", grey_val);
        }
        fprintf(pgm, "\r\n");
    }
}

static void accent_dungeon() {
    logger.t("Spiking Dungeon Out...");
    int i, j;
    int hardnesses[] = {ROCK_HARD, ROCK_MED, ROCK_SOFT};
    
    for(i = 0; i < 3; i++) {
        for(j = 0; j < POINT_LIMIT; j++) {
            int pos;
            do {
                pos = rand() % (DUNGEON_WIDTH * DUNGEON_HEIGHT);
            } while(_dungeon_array[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH]->content != tc_UNSET);
            tileAPI.update_hardness(_dungeon_array[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH], hardnesses[i]);
            tileAPI.update_content(_dungeon_array[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH], tc_ROCK);
        }
    }
    
    logger.t("Dungeon Spiked Out");
    
    if(DEBUG_MODE) {
        logger.t("Writing map to pgm");
        write_dungeon_pgm("rock_accent_map.pgm", 1);
    }
}

static void diffuse_dungeon() {
    logger.t("Diffusing Dungeon...");
    int points_hit = 3 * POINT_LIMIT;
    int i,j;
    int k, x_adj, y_adj, pass_bitmap = 0;
    int hardness;
    int x_coords[] = {0, 1, 0, -1, 1, 1, -1, -1};
    int y_coords[] = {-1, 0, 1, 0, -1, 1, 1, -1};
    while(points_hit < (DUNGEON_WIDTH * DUNGEON_HEIGHT)) {
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                // Look for i,j where we have already set a value
                if(_dungeon_array[i][j]->content == tc_UNSET) {
                    continue;
                }
                hardness = _dungeon_array[i][j]->rock_hardness;
                pass_bitmap = 0;
                for(k = 0; k < 8; k++) {
                    // Check if we will be in range;
                    x_adj = j + x_coords[k];
                    y_adj = i + y_coords[k];
                    if(x_adj < 0 || x_adj >= DUNGEON_WIDTH || y_adj < 0 || y_adj >= DUNGEON_HEIGHT) {
                        continue;
                    }
                    // Check if the tile has already been set or has had changes proposed
                    if(_dungeon_array[y_adj][x_adj]->content != tc_UNSET ||
                       tileAPI.are_changes_proposed(_dungeon_array[y_adj][x_adj])) {
                        continue;
                    }
                    
                    // Check if dependent changes have happened (for diagonals)
                    if((k < 4)                       || // an adjacent spot does not have dependency
                       (k == 4 && pass_bitmap >= 3)  || // diagonal checks
                       (k == 5 && pass_bitmap >= 6)  ||
                       (k == 6 && pass_bitmap >= 12) ||
                       (k == 7 && pass_bitmap >= 9)) {
                        // propose the new changes to the tile
                        tileAPI.propose_update_content(_dungeon_array[y_adj][x_adj], tc_ROCK);
                        tileAPI.propose_update_hardness(_dungeon_array[y_adj][x_adj], hardness);
                        
                        // update the pass bitmap
                        pass_bitmap |= (1 << k);
                    }
                }
            }
        }
        // commit the tile updates for this pass
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                tile_t* tile = _dungeon_array[i][j];
                // changes are different than the content, so we hit a point
                if(tile->content != tile->changes->content) {
                    points_hit++;
                }
                tileAPI.commit_updates(tile);
            }
        }
    }
    logger.t("Dungeon Diffused");
    
    if(DEBUG_MODE) {
        logger.t("Writing diffuse map out");
        write_dungeon_pgm("rock_diffuse_map.pgm", 1);
    }
}

static void smooth_dungeon() {
    logger.t("Smoothing Dungeon...");
    int i, j, k, x, y, sum, tiles_hit;
    int x_coords[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int y_coords[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            sum = 0;
            tiles_hit = 0;
            for(k = 0; k < 9; k++) {
                x = j + x_coords[k];
                y = i + y_coords[k];
                if(x < 0 || x >= DUNGEON_WIDTH || y < 0 || y >= DUNGEON_HEIGHT) {
                    continue;
                }
                sum += _dungeon_array[y][x]->rock_hardness;
                tiles_hit++;
            }
            tileAPI.propose_update_hardness(_dungeon_array[i][j], sum / tiles_hit);
        }
    }
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tileAPI.commit_updates(_dungeon_array[i][j]);
        }
    }
    
    logger.t("Dungeon Smoothed");
    
    if(DEBUG_MODE) {
        logger.t("Writing smooth map out");
        write_dungeon_pgm("rock_smooth_map.pgm", 0);
    }
}

static void border_dungeon() {
    logger.t("Bordering Dungeon...");
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        if(i == 0 || i == DUNGEON_HEIGHT - 1) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                tileAPI.update_content(_dungeon_array[i][j], tc_BORDER);
                tileAPI.update_hardness(_dungeon_array[i][j], ROCK_MAX);
            }
        } else {
            tileAPI.update_content(_dungeon_array[i][0], tc_BORDER);
            tileAPI.update_hardness(_dungeon_array[i][0], ROCK_MAX);
            tileAPI.update_content(_dungeon_array[i][DUNGEON_WIDTH - 1], tc_BORDER);
            tileAPI.update_hardness(_dungeon_array[i][DUNGEON_WIDTH - 1], ROCK_MAX);
        }
    }
    
    logger.t("Dungeon Bordered");
    
    if(DEBUG_MODE) {
        logger.t("Writing terrain map out");
        write_dungeon_pgm("rock_terrain_map.pgm", 0);
    }
}

static int is_open_space() {
    logger.t("Checking Open Space...");
    int i;
    int total_size = 0;
    for(i = 0; i < _room_size; i++) {
        total_size += (_room_array[i]->height * _room_array[i]->width);
    }
    logger.t("Room Space taken: %d out of total space %d", total_size, DUNGEON_WIDTH * DUNGEON_HEIGHT);
    return total_size < (DUNGEON_HEIGHT * DUNGEON_WIDTH * 0.20f);
}

static void add_rooms() {
    int i, j, k;
    // Rooms are created, add them to _dungeon_array
    for(i = 0; i < _room_size; i++) {
        for(j = _room_array[i]->location->y; j < _room_array[i]->location->y + _room_array[i]->height; j++) {
            for(k = _room_array[i]->location->x; k < _room_array[i]->location->x + _room_array[i]->width; k++) {
                tileAPI.update_content(_dungeon_array[j][k], tc_ROOM);
            }
        }
    }
}

dungeon_namespace const dungeonAPI = {
    dungeon_construct,
    dungeon_destruct,
    dungeon_generate_terrain,
    dungeon_place_rooms,
    dungeon_pathfind,
    dungeon_print
};