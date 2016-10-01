//
//  dungeon.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dungeon.h"
#include "corridor.h"
#include "pathfinder.h"
#include "../util/portable_endian.h"
#include "../graph/graph.h"
#include "../tile/tile.h"
#include "../room/room.h"
#include "../logger/logger.h"
#include "../env/env.h"

#define POINT_LIMIT (DUNGEON_HEIGHT*DUNGEON_WIDTH/25)

// Array of tiles for the dungeon
// size will be 21 rows x 80 cols

static point_t* player_pos;
static void accent_dungeon(dungeon_t* d);
static void diffuse_dungeon(dungeon_t* d);
static void smooth_dungeon(dungeon_t* d);
static void border_dungeon(dungeon_t* d);
static int  is_open_space(dungeon_t* d);
static void add_rooms(dungeon_t* d);
static void write_dungeon_pgm(dungeon_t* d, const char* file_name, int zone);
static void generate_terrain(dungeon_t* d);
static void place_rooms(dungeon_t* d);
static void pathfind(dungeon_t* d);
static void update_path_hardnesses(dungeon_t* d);

static void d_log_room(room_t* r) {
    if(DEBUG_MODE) {
        printf("Room: x: %2d, y: %2d, w: %2d, h: %2d\n", r->location->x, r->location->y, r->width, r->height);
    }
    logger.d("Room: x: %2d, y: %2d, w: %2d, h: %2d", r->location->x, r->location->y, r->width, r->height);
}

static dungeon_t* construct() {
    logger.i("Constructing Dungeon...");
    int i, j;
    srand((unsigned)time(NULL));
    
    dungeon_t* d = calloc(1, sizeof(dungeon_t));
    
    d->tiles = calloc(DUNGEON_HEIGHT, sizeof(*d->tiles));
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        d->tiles[i] = calloc(DUNGEON_WIDTH, sizeof(**d->tiles));
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            d->tiles[i][j] = tileAPI.construct(j, i);
        }
    }
    logger.i("Dungeon Constructed");
    return d;
}

static void destruct(dungeon_t* d) {
    logger.i("Destructing Dungeon...");
    int i, j;
    for(i = 0; i < d->room_size; i++) {
        roomAPI.destruct(d->rooms[i]);
    }
    free(d->rooms);
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tileAPI.destruct(d->tiles[i][j]);
        }
        free(d->tiles[i]);
    }
    free(d->tiles);
    free(d);
    logger.i("Dungeon Destructed");
}

static void generate(dungeon_t* d) {
    generate_terrain(d);
    place_rooms(d);
    pathfind(d);
    update_path_hardnesses(d);
    dungeonAPI.set_player_pos(d, NULL);
}

static void update_path_maps(dungeon_t* d) {
    point_t* p = dungeonAPI.get_player_pos();
    graph_t* g = pathfinderAPI.construct(d, 0);
    pathfinderAPI.generate_pathmap(g, d, p, 0);
    pathfinderAPI.destruct(g);
    g = pathfinderAPI.construct(d, 1);
    pathfinderAPI.generate_pathmap(g, d, p, 1);
    pathfinderAPI.destruct(g);
}

static void print(dungeon_t* d, int mode) {
    logger.i("Printing Dungeon mode: %d...", mode);
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            char c = tileAPI.char_for_content(d->tiles[i][j], mode);
            if(c == '?') {
                logger.e("Bad Tile Found @ (%2d, %2d) with content: %d", d->tiles[i][j]->location->x, d->tiles[i][j]->location->y, d->tiles[i][j]->content);
            }
            printf("%c", c);
        }
        printf("\n");
    }
    printf("\n");
    logger.i("Dungeon Printed");
}

static void load(dungeon_t* d) {
    logger.i("Loading Dungeon...");
    FILE* f;
    int version;
    int size;
    int i, j, k;
    int num_rooms = 0;
    uint8_t room_data[4];
    char* semantic = (char*)malloc(7*sizeof(char));
    uint8_t* hardness_map = (uint8_t*)malloc(DUNGEON_WIDTH*DUNGEON_HEIGHT*sizeof(uint8_t));
    
    f = fopen(LOAD_FILE, "rb");
    if(f == NULL) {
        logger.f("Dungeon save file could not be loaded: %s.  Exiting with error!", LOAD_FILE);
        free(semantic);
        free(hardness_map);
        exit(3);
    }
    
    fread(semantic, sizeof(char), 6, f);
    fread(&version, sizeof(int), 1, f);
    fread(&size, sizeof(int), 1, f);
    
    if(strcmp(semantic, "RLG327")) {
        logger.e("File %s is of a different format: %s, not RLG327! aborting now!", LOAD_FILE, semantic);
        free(semantic);
        free(hardness_map);
        fclose(f);
        exit(3);
    }
    
    version = be32toh(version);
    size = be32toh(size);
    num_rooms = (size - 1694) / 4;
    
    logger.i("Parsing file with version: %d, total size: %d", version, size);
    logger.i("Estimating the number of rooms: %d", num_rooms);
    
    fread(hardness_map, sizeof(uint8_t), DUNGEON_HEIGHT*DUNGEON_WIDTH, f);
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tileAPI.import_tile(d->tiles[i][j], hardness_map[i*DUNGEON_WIDTH + j], 0);
        }
    }
    
    d->room_size = num_rooms;
    d->rooms = calloc(d->room_size, sizeof(*d->rooms));
    for(i = 0; i < num_rooms; i++) {
        // read in room data
        fread(room_data, sizeof(uint8_t), 4, f);
        d->rooms[i] = roomAPI.construct(room_data[0], room_data[2], room_data[1], room_data[3]);
        
        // change room tiles to be rooms
        for(j = d->rooms[i]->location->y; j < d->rooms[i]->location->y + d->rooms[i]->height; j++) {
            for(k = d->rooms[i]->location->x; k < d->rooms[i]->location->x + d->rooms[i]->width; k++) {
                tileAPI.import_tile(d->tiles[j][k], d->tiles[j][k]->rock_hardness, 1);
            }
        }
    }
    dungeonAPI.set_player_pos(d, NULL);
    
    logger.i("Dungeon Loaded");
    free(semantic);
    free(hardness_map);
    fclose(f);
}

static void save(dungeon_t* d) {
    logger.i("Saving Dungeon...");
    FILE* f;
    int version = 0;
    int size = (d->room_size*4) + 1694;
    int i, j;
    uint8_t room_data[4];
    char* semantic = "RLG327";
    uint8_t* hardness_map = (uint8_t*)malloc(DUNGEON_WIDTH*DUNGEON_HEIGHT*sizeof(uint8_t));
    
    f = fopen(SAVE_FILE, "wb");
    if(f == NULL) {
        logger.f("Dungeon save file could not be opened: %s.  Aborting now", SAVE_FILE);
        free(hardness_map);
        fclose(f);
        exit(3);
    }
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            hardness_map[i*DUNGEON_WIDTH + j] = d->tiles[i][j]->rock_hardness;
        }
    }
    
    version = htobe32(version);
    size = htobe32(size);
    fwrite(semantic, sizeof(char), 6, f);
    fwrite(&version, sizeof(int), 1, f);
    fwrite(&size, sizeof(int), 1, f);
    fwrite(hardness_map, sizeof(uint8_t), DUNGEON_WIDTH*DUNGEON_HEIGHT, f);
    
    for(i = 0; i < d->room_size; i++) {
        roomAPI.export_room(d->rooms[i], room_data);
        fwrite(room_data, sizeof(uint8_t), 4, f);
    }
    
    logger.i("Dungeon Saved");
    
    free(hardness_map);
    fclose(f);
}

static void set_player_pos(dungeon_t* d, point_t* p) {
    // TODO: Check error bounds
    if(p == NULL) {
        // Generate random point or use starting values
        if(X_START < 80 && Y_START < 80) {
            player_pos = d->tiles[Y_START][X_START]->location;
        } else {
            int i = rand() % d->room_size;
            room_t* r = d->rooms[i];
            int j = rand() % r->width;
            int k = rand() % r->height;
            player_pos = d->tiles[r->location->y+k][r->location->x+j]->location;
        }
    } else {
        player_pos = p;
    }
}

static point_t* get_player_pos() {
    return player_pos;
}

static void generate_terrain(dungeon_t* d) {
    logger.i("Generating Terrain...");
    accent_dungeon(d);
    diffuse_dungeon(d);
    smooth_dungeon(d);
    border_dungeon(d);
    logger.i("Terrain Generated");
    
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            if(d->tiles[i][j]->content == tc_UNSET) {
                logger.f("Unset Tile @ (%2d, %2d) after terrain generation!", j, i);
                exit(2);
            }
        }
    }
}

static void place_rooms(dungeon_t* d) {
    logger.i("Placing rooms in dungeon...");
    int i, j;
    int x, y, width, height, room_valid = 1;
    int overlap_valid = 1;
    int place_attempts_fail = 0;
    d->room_size = 6;
    d->rooms = calloc(d->room_size, sizeof(*d->rooms));
    
    // initally create the 6 rooms and check if overlap is valid.
    logger.d("Generating initial rooms...");
    do {
        for(i = 0; i < d->room_size; i++) {
            do {
                room_valid = 1;
                x = (rand() % (DUNGEON_WIDTH - 2)) + 1;
                y = (rand() % (DUNGEON_HEIGHT - 2)) + 1;
                width = (rand() % 12) + 4;
                height = (rand() % 10) + 3;
                if(((x + width) > DUNGEON_WIDTH - 2) ||
                   ((y + height) > DUNGEON_HEIGHT - 2)) {
                    room_valid = 0;
                }
            } while(!room_valid);
            d->rooms[i] = roomAPI.construct(x, y, width, height);
        }
        
        overlap_valid = 1;
        for(i = 0; i < d->room_size; i++) {
            for(j = 0; j < i; j++) {
                if(roomAPI.is_overlap(d->rooms[i], d->rooms[j])) {
                    overlap_valid = 0;
                    break;
                }
            }
            if(!overlap_valid) {
                // cleanup rooms, we don't have a valid room series
                for(j = 0; j < d->room_size; j++) {
                    roomAPI.destruct(d->rooms[j]);
                }
                break;
            }
        }
    } while(!overlap_valid);
    logger.d("Initial Rooms Generated");
    
    // Check if we have open space and add rooms one at a time
    logger.d("Checking on adding more rooms");
    while(is_open_space(d) && place_attempts_fail < 2000) {
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
        for(i = 0; i < d->room_size; i++) {
            if(roomAPI.is_overlap(new_room, d->rooms[i])) {
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
            room_t** new_room_array = (room_t**) realloc(d->rooms, (d->room_size + 1) * sizeof(*d->rooms));
            if(new_room_array == NULL) {
                logger.e("Room Array Reallocation failed!");
                break;
            }
            d->rooms = new_room_array;
            d->rooms[d->room_size++] = new_room;
            logger.t("Room added");
        }
    }
    
    if(place_attempts_fail >= 2000) {
        logger.d("Stopping room placement due to attempt failures: %d", place_attempts_fail);
    }
    
    add_rooms(d);
    
    for(i = 0; i < d->room_size; i++) {
        d_log_room(d->rooms[i]);
    }
    
    logger.i("Rooms Placed in Dungeon");
}

static void pathfind(dungeon_t* d) {
    logger.i("Generating Corridors...");
    
    graph_t* g = corridorAPI.construct(d, 0);
    point_t src;
    point_t dest;
    int num_paths = 0;
    int i;
    for(i = 0; i < d->room_size - 1; i++) {
        if(d->rooms[i]->connected && d->rooms[i+1]->connected) {
            logger.t("Room %d and %d are already connected, moving to next path", i, i+1);
            continue;
        }
        
        src.x = (rand() % d->rooms[i]->width) + d->rooms[i]->location->x;
        src.y = (rand() % d->rooms[i]->height) + d->rooms[i]->location->y;
        dest.x = (rand() % d->rooms[i+1]->width) + d->rooms[i+1]->location->x;
        dest.y = (rand() % d->rooms[i+1]->height) + d->rooms[i+1]->location->y;
        logger.d("Room Path %2d: Routing from (%2d, %2d) to (%2d, %2d)", i, src.x, src.y, dest.x, dest.y);
        corridorAPI.pathfind(g, d, &src, &dest);
        d->rooms[i]->connected = 1;
        d->rooms[i+1]->connected = 1;
        num_paths++;
    }
    corridorAPI.destruct(g);
    
    // Connect last room to first room
    g = corridorAPI.construct(d, 1);
    
    src.x = (rand() % d->rooms[d->room_size - 1]->width) + d->rooms[d->room_size - 1]->location->x;
    src.y = (rand() % d->rooms[d->room_size - 1]->height) + d->rooms[d->room_size - 1]->location->y;
    dest.x = (rand() % d->rooms[0]->width) + d->rooms[0]->location->x;
    dest.y = (rand() % d->rooms[0]->height) + d->rooms[0]->location->y;
    logger.d("Room Path %2d: Routing from (%2d, %2d) to (%2d, %2d)", i, src.x, src.y, dest.x, dest.y);
    corridorAPI.pathfind(g, d, &src, &dest);
    num_paths++;
    
    corridorAPI.destruct(g);
    
    logger.i("Generated %d paths", num_paths);
    logger.i("Corridors Generated");
}

static void update_path_hardnesses(dungeon_t* d) {
    int i, j;
    tile_t* t;
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            t = d->tiles[i][j];
            if(t->content == tc_ROOM || t->content == tc_PATH) {
                tileAPI.update_hardness(t, 0);
            }
        }
    }
}

static void write_dungeon_pgm(dungeon_t* d, const char* file_name, int zone) {
    int i, j;
    FILE* pgm = fopen(file_name, "w+");
    
    fprintf(pgm, "P2 \r\n%d %d \r\n255 \r\n", DUNGEON_WIDTH, DUNGEON_HEIGHT);
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            int hardness = d->tiles[i][j]->rock_hardness;
            int grey_val = hardness == ROCK_HARD ? 255 :
            hardness == ROCK_MED ? 170 :
            hardness == ROCK_SOFT ? 80 : 0;
            grey_val = zone ? grey_val : hardness;
            fprintf(pgm, "%d ", grey_val);
        }
        fprintf(pgm, "\r\n");
    }
}

static void accent_dungeon(dungeon_t* d) {
    logger.t("Spiking Dungeon Out...");
    int i, j;
    int hardnesses[] = {ROCK_HARD, ROCK_MED, ROCK_SOFT};
    
    for(i = 0; i < 3; i++) {
        for(j = 0; j < POINT_LIMIT; j++) {
            int pos;
            do {
                pos = rand() % (DUNGEON_WIDTH * DUNGEON_HEIGHT);
            } while(d->tiles[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH]->content != tc_UNSET);
            tileAPI.update_hardness(d->tiles[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH], hardnesses[i]);
            tileAPI.update_content(d->tiles[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH], tc_ROCK);
        }
    }
    
    logger.t("Dungeon Spiked Out");
    
    if(DEBUG_MODE) {
        logger.t("Writing map to pgm");
        write_dungeon_pgm(d, "rock_accent_map.pgm", 1);
    }
}

static void diffuse_dungeon(dungeon_t* d) {
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
                if(d->tiles[i][j]->content == tc_UNSET) {
                    continue;
                }
                hardness = d->tiles[i][j]->rock_hardness;
                pass_bitmap = 0;
                for(k = 0; k < 8; k++) {
                    // Check if we will be in range;
                    x_adj = j + x_coords[k];
                    y_adj = i + y_coords[k];
                    if(x_adj < 0 || x_adj >= DUNGEON_WIDTH || y_adj < 0 || y_adj >= DUNGEON_HEIGHT) {
                        continue;
                    }
                    // Check if the tile has already been set or has had changes proposed
                    if(d->tiles[y_adj][x_adj]->content != tc_UNSET ||
                       tileAPI.are_changes_proposed(d->tiles[y_adj][x_adj])) {
                        continue;
                    }
                    
                    // Check if dependent changes have happened (for diagonals)
                    if((k < 4)                       || // an adjacent spot does not have dependency
                       (k == 4 && pass_bitmap >= 3)  || // diagonal checks
                       (k == 5 && pass_bitmap >= 6)  ||
                       (k == 6 && pass_bitmap >= 12) ||
                       (k == 7 && pass_bitmap >= 9)) {
                        // propose the new changes to the tile
                        tileAPI.propose_update_content(d->tiles[y_adj][x_adj], tc_ROCK);
                        tileAPI.propose_update_hardness(d->tiles[y_adj][x_adj], hardness);
                        
                        // update the pass bitmap
                        pass_bitmap |= (1 << k);
                    }
                }
            }
        }
        // commit the tile updates for this pass
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                tile_t* tile = d->tiles[i][j];
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
        write_dungeon_pgm(d, "rock_diffuse_map.pgm", 1);
    }
}

static void smooth_dungeon(dungeon_t* d) {
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
                sum += d->tiles[y][x]->rock_hardness;
                tiles_hit++;
            }
            tileAPI.propose_update_hardness(d->tiles[i][j], sum / tiles_hit);
        }
    }
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tileAPI.commit_updates(d->tiles[i][j]);
        }
    }
    
    logger.t("Dungeon Smoothed");
    
    if(DEBUG_MODE) {
        logger.t("Writing smooth map out");
        write_dungeon_pgm(d, "rock_smooth_map.pgm", 0);
    }
}

static void border_dungeon(dungeon_t* d) {
    logger.t("Bordering Dungeon...");
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        if(i == 0 || i == DUNGEON_HEIGHT - 1) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                tileAPI.update_content(d->tiles[i][j], tc_BORDER);
                tileAPI.update_hardness(d->tiles[i][j], ROCK_MAX);
            }
        } else {
            tileAPI.update_content(d->tiles[i][0], tc_BORDER);
            tileAPI.update_hardness(d->tiles[i][0], ROCK_MAX);
            tileAPI.update_content(d->tiles[i][DUNGEON_WIDTH - 1], tc_BORDER);
            tileAPI.update_hardness(d->tiles[i][DUNGEON_WIDTH - 1], ROCK_MAX);
        }
    }
    
    logger.t("Dungeon Bordered");
    
    if(DEBUG_MODE) {
        logger.t("Writing terrain map out");
        write_dungeon_pgm(d, "rock_terrain_map.pgm", 0);
    }
}

static int is_open_space(dungeon_t* d) {
    logger.t("Checking Open Space...");
    int i;
    int total_size = 0;
    for(i = 0; i < d->room_size; i++) {
        total_size += (d->rooms[i]->height * d->rooms[i]->width);
    }
    logger.t("Room Space taken: %d out of total space %d", total_size, DUNGEON_WIDTH * DUNGEON_HEIGHT);
    return total_size < (DUNGEON_HEIGHT * DUNGEON_WIDTH * 0.20f);
}

static void add_rooms(dungeon_t* d) {
    int i, j, k;
    // Rooms are created, add them to _dungeon_array
    for(i = 0; i < d->room_size; i++) {
        for(j = d->rooms[i]->location->y; j < d->rooms[i]->location->y + d->rooms[i]->height; j++) {
            for(k = d->rooms[i]->location->x; k < d->rooms[i]->location->x + d->rooms[i]->width; k++) {
                tileAPI.update_content(d->tiles[j][k], tc_ROOM);
            }
        }
    }
}

dungeon_namespace const dungeonAPI = {
    construct,
    destruct,
    generate,
    update_path_maps,
    print,
    load,
    save,
    set_player_pos,
    get_player_pos
};
