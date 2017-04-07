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
#include <ncurses.h>

#include "dungeon.h"
#include "corridor.h"
#include "pathfinder.h"
#include "../util/portable_endian.h"
#include "../graph/graph.h"
#include "../tile/tile.h"
#include "../room/room.h"
#include "../logger/logger.h"
#include "../env/env.h"
#include "../character/character.h"

#define POINT_LIMIT (DUNGEON_HEIGHT*DUNGEON_WIDTH/25)

// Private variables
static dungeon_t* _base_dungeon = NULL;

// Public Functions

static void update_path_maps_impl(dungeon_t* d);
static void print_impl(dungeon_t* d, int mode);
static void printn_impl(dungeon_t* d, int mode);
static void load_impl(dungeon_t* d);
static void save_impl(dungeon_t* d);
static void rand_point_impl(dungeon_t* d, point_t* p);

// Private Functions

static void accent_dungeon(dungeon_t* d);
static void diffuse_dungeon(dungeon_t* d);
static void smooth_dungeon(dungeon_t* d);
static void border_dungeon(dungeon_t* d);
static int  is_open_space(dungeon_t* d);
static void add_rooms(dungeon_t* d);
static void write_dungeon_pgm(dungeon_t* d, const char* file_name, int zone);
static void generate_terrain(dungeon_t* d);
static void place_rooms(dungeon_t* d);
static void place_staircases(dungeon_t* d);
static void pathfind(dungeon_t* d);
static void update_path_hardnesses(dungeon_t* d);

static void d_log_room(room_t* r) {
    if(DEBUG_MODE && !NCURSES_MODE) {
        printf("Room: x: %2d, y: %2d, w: %2d, h: %2d\n", r->location->x, r->location->y, r->width, r->height);
    }
    logger.d("Room: x: %2d, y: %2d, w: %2d, h: %2d", r->location->x, r->location->y, r->width, r->height);
}

static dungeon_t* get_dungeon_impl() {
    if(_base_dungeon == NULL) {
        _base_dungeon = dungeonAPI.construct();
    }
    return _base_dungeon;
}
    
static void teardown_dungeon_impl() {
    if(_base_dungeon != NULL) {
        dungeonAPI.destruct(_base_dungeon);
        _base_dungeon = NULL;
    }
}

static dungeon_t* construct_impl() {
    logger.i("Constructing Dungeon...");
    int i, j;
    srand((unsigned)time(NULL));
    
    dungeon_t* d = (dungeon_t*)calloc(1, sizeof(dungeon_t));
    
    d->tiles = (tile_t***)calloc(DUNGEON_HEIGHT, sizeof(*d->tiles));
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        d->tiles[i] = (tile_t**)calloc(DUNGEON_WIDTH, sizeof(**d->tiles));
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            d->tiles[i][j] = tileAPI.construct(j, i);
        }
    }
    
    d->tunnel_map = NULL;
    d->non_tunnel_map = NULL;
    
    d->update_path_maps = update_path_maps_impl;
    d->print = print_impl;
    d->printn = printn_impl;
    d->load = load_impl;
    d->save = save_impl;
    logger.i("Dungeon Constructed");
    return d;
}

static void destruct_impl(dungeon_t* d) {
    logger.i("Destructing Dungeon...");
    int i, j;
    for(i = 0; i < d->room_size; i++) {
        roomAPI.destruct(d->rooms[i]);
    }
    free(d->rooms);
    
    if(d->non_tunnel_map != NULL) {
        pathfinderAPI.destruct(d->non_tunnel_map);
        d->non_tunnel_map = NULL;
    }
    if(d->tunnel_map != NULL) {
        pathfinderAPI.destruct(d->tunnel_map);
        d->tunnel_map = NULL;
    }
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tileAPI.destruct(d->tiles[i][j]);
        }
        free(d->tiles[i]);
    }
    free(d->tiles);
    free(d);
    if(d == _base_dungeon) {
        _base_dungeon = NULL;
    }
    logger.i("Dungeon Destructed");
}

static dungeon_t* move_floors_impl() {
    int i, j;
    int pc_placed = 0;
    character_t* pc = characterAPI.get_pc();
    destruct_impl(_base_dungeon);
    _base_dungeon = dungeonAPI.construct();
    dungeonAPI.generate(_base_dungeon);
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            if(STAIR_FLAG == 1 && _base_dungeon->tiles[i][j]->content == tc_DNSTR) { // pc went upstairs, so it should be placed on the down stairs
                pc->set_position(pc, _base_dungeon->tiles[i][j]->location);
                pc_placed = 1;
                break;
            } else if(STAIR_FLAG == 2 && _base_dungeon->tiles[i][j]->content == tc_UPSTR) { // pc went downstairs, so it should be placed on the up stairs
                pc->set_position(pc, _base_dungeon->tiles[i][j]->location);
                pc_placed = 1;
                break;
            }
        }
        if(pc_placed) {
            break;
        }
    }
    _base_dungeon->update_path_maps(_base_dungeon);
    return _base_dungeon;
}

static void generate_impl(dungeon_t* d) {
    generate_terrain(d);
    place_rooms(d);
    place_staircases(d);
    pathfind(d);
    update_path_hardnesses(d);
}

static void update_path_maps_impl(dungeon_t* d) {
    point_t* p;
    p = characterAPI.get_pc()->position;
    if(d->tunnel_map == NULL) {
        d->tunnel_map = pathfinderAPI.construct(d, 1);
    }
    if(d->non_tunnel_map == NULL) {
        d->non_tunnel_map = pathfinderAPI.construct(d, 0);
    }
    
    int error1 = pathfinderAPI.generate_pathmap(d->non_tunnel_map, d, p, 0);
    if(error1) {
        // need to regenerate the graph
        pathfinderAPI.destruct(d->non_tunnel_map);
        d->non_tunnel_map = pathfinderAPI.construct(d, 0);
    }
    int error2 = pathfinderAPI.generate_pathmap(d->tunnel_map, d, p, 1);
    if(error2) {
        // need to regenerate the graph
        pathfinderAPI.destruct(d->non_tunnel_map);
        d->non_tunnel_map = pathfinderAPI.construct(d, 0);
    }
}

static void print_impl(dungeon_t* d, int mode) {
    int i, j;
    logger.i("Printing Dungeon mode: %d...", mode);
    if(NCURSES_MODE) {
        d->printn(d, mode);
    } else {
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                char c = d->tiles[i][j]->char_for_content(d->tiles[i][j], mode);
                if(c == '?') {
                    logger.e("Bad Tile Found @ (%2d, %2d) with content: %d", d->tiles[i][j]->location->x, d->tiles[i][j]->location->y, d->tiles[i][j]->content);
                }
                printf("%c", c);
            }
            printf("\n");
        }
        printf("\n");
    }
    logger.i("Dungeon Printed");
}

static void printn_impl(dungeon_t* d, int mode) {
    logger.i("NCURSES: Printing Dungeon mode: %d...", mode);
    clear();
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            char c = d->tiles[i][j]->char_for_content(d->tiles[i][j], mode);
            if(c == '?') {
                logger.e("Bad Tile Found @ (%2d, %2d) with content: %d", d->tiles[i][j]->location->x, d->tiles[i][j]->location->y, d->tiles[i][j]->content);
            }
            mvaddch(i+1, j, c);
        }
    }
    refresh();
    logger.i("Dungeon Printed");
}

static void load_impl(dungeon_t* d) {
    logger.i("Loading Dungeon...");
    FILE* f;
    int version;
    int size;
    int i, j, k;
    int num_rooms = 0;
    uint8_t room_data[4];
    char* semantic = (char*)calloc(7, sizeof(char));
    uint8_t* hardness_map = (uint8_t*)malloc(DUNGEON_WIDTH*DUNGEON_HEIGHT*sizeof(uint8_t));
    
    f = fopen(LOAD_FILE, "rb");
    if(f == NULL) {
        logger.f("Dungeon save file could not be loaded: %s.  Exiting with error!", LOAD_FILE);
        free(semantic);
        free(hardness_map);
        envAPI.cleanup();
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
        envAPI.cleanup();
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
    d->rooms = (room_t**)calloc(d->room_size, sizeof(*d->rooms));
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
    
    place_staircases(d);
    
    logger.i("Dungeon Loaded");
    free(semantic);
    free(hardness_map);
    fclose(f);
}

static void save_impl(dungeon_t* d) {
    logger.i("Saving Dungeon...");
    FILE* f;
    int version = 0;
    int size = (d->room_size*4) + 1694;
    int i, j;
    room_t* r;
    uint8_t room_data[4];
    const char* semantic = "RLG327";
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
        r = d->rooms[i];
        r->export_room(r, room_data);
        fwrite(room_data, sizeof(uint8_t), 4, f);
    }
    
    logger.i("Dungeon Saved");
    
    free(hardness_map);
    fclose(f);
}

static void rand_point_impl(dungeon_t* d, point_t* p) {
    int i, j, k;
    room_t* r;
    point_t* tile_loc;
    i = rand() % d->room_size;
    r = d->rooms[i];
    j = rand() % r->width;
    k = rand() % r->height;
    tile_loc = d->tiles[r->location->y+k][r->location->x+j]->location;
    p->x = tile_loc->x;
    p->y = tile_loc->y;
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
    d->rooms = (room_t**)calloc(d->room_size, sizeof(*d->rooms));
    
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
                if(d->rooms[i]->is_overlap(d->rooms[i], d->rooms[j])) {
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
            if(new_room->is_overlap(new_room, d->rooms[i])) {
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

static void place_staircases(dungeon_t* d) {
    logger.i("Placing staircases");
    
    point_t* up = pointAPI.construct(0, 0);
    rand_point_impl(d, up);
    tile_t* upstairs = d->tiles[up->y][up->x];
    upstairs->update_content(upstairs, tc_UPSTR);
    
    point_t* down = pointAPI.construct(0, 0);
    do {
        rand_point_impl(d, down);
    } while(up->distance(up, down) == 0);
    
    tile_t* downstairs = d->tiles[down->y][down->x];
    downstairs->update_content(downstairs, tc_DNSTR);
    
    pointAPI.destruct(up);
    pointAPI.destruct(down);
    
    logger.i("Staircases placed in dungeon");
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
                t->update_hardness(t, 0);
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
    tile_t* t;
    int hardnesses[] = {ROCK_HARD, ROCK_MED, ROCK_SOFT};
    
    for(i = 0; i < 3; i++) {
        for(j = 0; j < POINT_LIMIT; j++) {
            int pos;
            do {
                pos = rand() % (DUNGEON_WIDTH * DUNGEON_HEIGHT);
            } while(d->tiles[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH]->content != tc_UNSET);
            t = d->tiles[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH];
            t->update_hardness(t, hardnesses[i]);
            t->update_content(t, tc_ROCK);
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
    tile_t* t;
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
                    t = d->tiles[y_adj][x_adj];
                    if(t->content != tc_UNSET ||
                       t->are_changes_proposed(t)) {
                        continue;
                    }
                    
                    // Check if dependent changes have happened (for diagonals)
                    if((k < 4)                       || // an adjacent spot does not have dependency
                       (k == 4 && pass_bitmap >= 3)  || // diagonal checks
                       (k == 5 && pass_bitmap >= 6)  ||
                       (k == 6 && pass_bitmap >= 12) ||
                       (k == 7 && pass_bitmap >= 9)) {
                        // propose the new changes to the tile
                        t->propose_update_content(t, tc_ROCK);
                        t->propose_update_hardness(t, hardness);
                        
                        // update the pass bitmap
                        pass_bitmap |= (1 << k);
                    }
                }
            }
        }
        // commit the tile updates for this pass
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                tile_t* t = d->tiles[i][j];
                // changes are different than the content, so we hit a point
                if(t->content != t->changes->content) {
                    points_hit++;
                }
                t->commit_updates(t);
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
            d->tiles[i][j]->propose_update_hardness(d->tiles[i][j], sum / tiles_hit);
        }
    }
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            d->tiles[i][j]->commit_updates(d->tiles[i][j]);
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
    tile_t* t;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        if(i == 0 || i == DUNGEON_HEIGHT - 1) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                t = d->tiles[i][j];
                t->update_content(t, tc_BORDER);
                t->update_hardness(t, ROCK_MAX);
            }
        } else {
            t = d->tiles[i][0];
            t->update_content(t, tc_BORDER);
            t->update_hardness(t, ROCK_MAX);
            
            t = d->tiles[i][DUNGEON_WIDTH - 1];
            t->update_content(t, tc_BORDER);
            t->update_hardness(t, ROCK_MAX);
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
                d->tiles[j][k]->update_content(d->tiles[j][k], tc_ROOM);
            }
        }
    }
}

dungeon_namespace const dungeonAPI = {
    get_dungeon_impl,
    teardown_dungeon_impl,
    construct_impl,
    move_floors_impl,
    destruct_impl,
    generate_impl,
    rand_point_impl
};
