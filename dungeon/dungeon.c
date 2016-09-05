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

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80
#define POINT_LIMIT (DUNGEON_HEIGHT*DUNGEON_WIDTH/25)
#define ROCK_HARD 254
#define ROCK_MED  127
#define ROCK_SOFT 1

// Array of tiles for the dungeon
// size will be 21 rows x 80 cols
tile_t*** _dungeon_array;

static void write_dungeon_pgm(const char* file_name, int zone);
static void accent_dungeon();
static void diffuse_dungeon();
static void smooth_dungeon();

void dungeon_construct() {
    int i, j;
    srand((unsigned)time(NULL));
    
    _dungeon_array = calloc(DUNGEON_HEIGHT, sizeof(*_dungeon_array));
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        _dungeon_array[i] = calloc(DUNGEON_WIDTH, sizeof(**_dungeon_array));
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            _dungeon_array[i][j] = tileAPI.construct(j, i);
        }
    }
}

void dungeon_destruct() {
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tileAPI.destruct(_dungeon_array[i][j]);
        }
        free(_dungeon_array[i]);
    }
    free(_dungeon_array);
}

void dungeon_generate_terrain() {
    accent_dungeon();
    diffuse_dungeon();
    smooth_dungeon();
}

void dungeon_place_rooms() {
    
}

void dungeon_pathfind() {
    
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
    
    write_dungeon_pgm("rock_accent_map.pgm", 1);
}

static void diffuse_dungeon() {
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
    write_dungeon_pgm("rock_diffuse_map.pgm", 1);
}

static void smooth_dungeon() {
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
    write_dungeon_pgm("rock_smooth_map.pgm", 0);
}

dungeon_namespace const dungeonAPI = {
    dungeon_construct,
    dungeon_destruct,
    dungeon_generate_terrain,
    dungeon_place_rooms,
    dungeon_pathfind
};