//
//  dungeon.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "../character/character_store.h"

#define POINT_LIMIT (DUNGEON_HEIGHT*DUNGEON_WIDTH/25)

dungeon* dungeon::_base = NULL;

void dungeon::d_log_room(room* r) {
    if(env_constants::DEBUG_MODE && !env_constants::NCURSES_MODE) {
        printf("Room: x: %2d, y: %2d, w: %2d, h: %2d\n", r->location->x, r->location->y, r->width, r->height);
    }
    logger::d("Room: x: %2d, y: %2d, w: %2d, h: %2d", r->location->x, r->location->y, r->width, r->height);
}

dungeon* dungeon::get_dungeon() {
    if(_base == NULL) {
        _base = new dungeon();
    }
    return _base;
}

void dungeon::teardown() {
    if(_base != NULL) {
        delete _base;
        _base = NULL;
    }
}

dungeon::dungeon() {
    logger::i("Constructing Dungeon...");
    int i, j;
    tiles = (tile***)calloc(DUNGEON_HEIGHT, sizeof(*tiles));
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        tiles[i] = (tile**)calloc(DUNGEON_WIDTH, sizeof(**tiles));
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tiles[i][j] = new tile((uint8_t)j, (uint8_t)i);
        }
    }
    
    rooms = NULL;
    room_size = 0;
    tunnel_map = NULL;
    non_tunnel_map = NULL;
    logger::i("Dungeon Constructed");
}

dungeon::~dungeon() {
    logger::i("Destructing Dungeon...");
    int i, j;
    for(i = 0; i < room_size; i++) {
        delete rooms[i];
    }
    free(rooms);
    
    if(non_tunnel_map != NULL) {
        pathfinder::destruct(non_tunnel_map);
        non_tunnel_map = NULL;
    }
    if(tunnel_map != NULL) {
        pathfinder::destruct(tunnel_map);
        tunnel_map = NULL;
    }
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            delete tiles[i][j];
        }
        free(tiles[i]);
    }
    free(tiles);
    if(this == _base) {
        _base = NULL;
    }
    logger::i("Dungeon Destructed");
}

dungeon* dungeon::move_floors() {
    int i, j;
    int pc_placed = 0;
    character* pc = character::get_pc();
    delete _base;
    _base = new dungeon();
    _base->generate();
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            if(env_constants::STAIR_FLAG == 1 && _base->tiles[i][j]->content == tc_DNSTR) { // pc went upstairs, so it should be placed on the down stairs
                pc->set_position(_base->tiles[i][j]->location);
                pc_placed = 1;
                break;
            } else if(env_constants::STAIR_FLAG == 2 && _base->tiles[i][j]->content == tc_UPSTR) { // pc went downstairs, so it should be placed on the up stairs
                pc->set_position(_base->tiles[i][j]->location);
                pc_placed = 1;
                break;
            }
        }
        if(pc_placed) {
            break;
        }
    }
    _base->update_path_maps();
    return _base;
}

void dungeon::generate() {
    generate_terrain();
    place_rooms();
    place_staircases();
    pathfind();
    update_path_hardnesses();
}

void dungeon::update_path_maps() {
    point* p = character::get_pc()->position;
    if(tunnel_map == NULL) {
        tunnel_map = pathfinder::construct(this, 1);
    }
    if(non_tunnel_map == NULL) {
        non_tunnel_map = pathfinder::construct(this, 0);
    }
    
    int error1 = pathfinder::generate_pathmap(non_tunnel_map, this, p, 0);
    if(error1) {
        // need to regenerate the graph
        pathfinder::destruct(non_tunnel_map);
        non_tunnel_map = pathfinder::construct(this, 0);
    }
    int error2 = pathfinder::generate_pathmap(tunnel_map, this, p, 1);
    if(error2) {
        // need to regenerate the graph
        pathfinder::destruct(non_tunnel_map);
        non_tunnel_map = pathfinder::construct(this, 0);
    }
}

void dungeon::print(int mode) {
    int i, j;
    logger::i("Printing Dungeon mode: %d...", mode);
    if(env_constants::NCURSES_MODE) {
        printn(mode);
    } else {
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                char c = tiles[i][j]->char_for_content(mode);
                if(c == '?') {
                    logger::e("Bad Tile Found @ (%2d, %2d) with content: %d", tiles[i][j]->location->x, tiles[i][j]->location->y, tiles[i][j]->content);
                }
                printf("%c", c);
            }
            printf("\n");
        }
        printf("\n");
    }
    logger::i("Dungeon Printed");
}

void dungeon::printn(int mode) {
    logger::i("NCURSES: Printing Dungeon mode: %d...", mode);
    clear();
    int i, j;
    attron(COLOR_PAIR(COLOR_BLACK));
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            char c = tiles[i][j]->char_for_content(mode);
            if(c == '?') {
                logger::e("Bad Tile Found @ (%2d, %2d) with content: %d", tiles[i][j]->location->x, tiles[i][j]->location->y, tiles[i][j]->content);
            }
            mvaddch(i+1, j, c);
        }
    }
    attroff(COLOR_PAIR(COLOR_BLACK));
    character_id_t* alive_npcs = character_store::get_alive_characters();
    for(i = 0; i < character_store::CHARACTER_COUNT; i++) {
        character* c = character_store::npc_for_id(alive_npcs[i]);
        char content = c->get_print_symb(mode);
        if(content != c->symb) {
            attron(COLOR_PAIR(COLOR_BLACK));
            mvaddch(c->position->y + 1, c->position->x, content);
            attroff(COLOR_PAIR(COLOR_BLACK));
        } else {
            attron(COLOR_PAIR(c->color));
            mvaddch(c->position->y + 1, c->position->x, content);
            attroff(COLOR_PAIR(c->color));
        }
    }
    character* pc = character::get_pc();
    mvaddch(pc->position->y+1, pc->position->x, pc->symb);
    mvaddch(0, 0, ' ');
    refresh();
    logger::i("Dungeon Printed");
}

void dungeon::load() {
    logger::i("Loading Dungeon...");
    FILE* f;
    int version;
    int size;
    int i, j, k;
    int num_rooms = 0;
    uint8_t room_data[4];
    char* semantic = (char*)calloc(7, sizeof(char));
    uint8_t* hardness_map = (uint8_t*)malloc(DUNGEON_WIDTH*DUNGEON_HEIGHT*sizeof(uint8_t));
    
    f = fopen(env_constants::LOAD_FILE, "rb");
    if(f == NULL) {
        logger::f("Dungeon save file could not be loaded: %s.  Exiting with error!", env_constants::LOAD_FILE);
        free(semantic);
        free(hardness_map);
        env::cleanup();
        exit(3);
    }
    
    fread(semantic, sizeof(char), 6, f);
    fread(&version, sizeof(int), 1, f);
    fread(&size, sizeof(int), 1, f);
    
    if(strcmp(semantic, "RLG327")) {
        logger::e("File %s is of a different format: %s, not RLG327! aborting now!", env_constants::LOAD_FILE, semantic);
        free(semantic);
        free(hardness_map);
        fclose(f);
        env::cleanup();
        exit(3);
    }
    
    version = be32toh(version);
    size = be32toh(size);
    num_rooms = (size - 1694) / 4;
    
    logger::i("Parsing file with version: %d, total size: %d", version, size);
    logger::i("Estimating the number of rooms: %d", num_rooms);
    
    fread(hardness_map, sizeof(uint8_t), DUNGEON_HEIGHT*DUNGEON_WIDTH, f);
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tiles[i][j] = new tile(hardness_map[i*DUNGEON_WIDTH + j], false);
        }
    }
    
    room_size = num_rooms;
    rooms = (room**)calloc(room_size, sizeof(*rooms));
    for(i = 0; i < num_rooms; i++) {
        // read in room data
        fread(room_data, sizeof(uint8_t), 4, f);
        rooms[i] = new room(room_data[0], room_data[2], room_data[1], room_data[3]);
        
        // change room tiles to be rooms
        for(j = rooms[i]->location->y; j < rooms[i]->location->y + rooms[i]->height; j++) {
            for(k = rooms[i]->location->x; k < rooms[i]->location->x + rooms[i]->width; k++) {
                tiles[j][k] = new tile(tiles[j][k]->rock_hardness, true);
            }
        }
    }
    
    place_staircases();
    
    logger::i("Dungeon Loaded");
    free(semantic);
    free(hardness_map);
    fclose(f);
}

void dungeon::save() {
    logger::i("Saving Dungeon...");
    FILE* f;
    int version = 0;
    int size = (room_size*4) + 1694;
    int i, j;
    room* r;
    uint8_t room_data[4];
    const char* semantic = "RLG327";
    uint8_t* hardness_map = (uint8_t*)malloc(DUNGEON_WIDTH*DUNGEON_HEIGHT*sizeof(uint8_t));
    
    f = fopen(env_constants::SAVE_FILE, "wb");
    if(f == NULL) {
        logger::f("Dungeon save file could not be opened: %s.  Aborting now", env_constants::SAVE_FILE);
        free(hardness_map);
        fclose(f);
        exit(3);
    }
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            hardness_map[i*DUNGEON_WIDTH + j] = tiles[i][j]->rock_hardness;
        }
    }
    
    version = htobe32(version);
    size = htobe32(size);
    fwrite(semantic, sizeof(char), 6, f);
    fwrite(&version, sizeof(int), 1, f);
    fwrite(&size, sizeof(int), 1, f);
    fwrite(hardness_map, sizeof(uint8_t), DUNGEON_WIDTH*DUNGEON_HEIGHT, f);
    
    for(i = 0; i < room_size; i++) {
        r = rooms[i];
        r->export_room(room_data);
        fwrite(room_data, sizeof(uint8_t), 4, f);
    }
    
    logger::i("Dungeon Saved");
    
    free(hardness_map);
    fclose(f);
}

void dungeon::rand_point(dungeon* d, point* p) {
    int i, j, k;
    room* r;
    point* tile_loc;
    i = rand() % d->room_size;
    r = d->rooms[i];
    j = rand() % r->width;
    k = rand() % r->height;
    tile_loc = d->tiles[r->location->y+k][r->location->x+j]->location;
    p->x = tile_loc->x;
    p->y = tile_loc->y;
}

void dungeon::generate_terrain() {
    logger::i("Generating Terrain...");
    accent();
    diffuse();
    smooth();
    add_borders();
    logger::i("Terrain Generated");
    
    int i, j;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            if(tiles[i][j]->content == tc_UNSET) {
                logger::f("Unset Tile @ (%2d, %2d) after terrain generation!", j, i);
                exit(2);
            }
        }
    }
}

void dungeon::place_rooms() {
    logger::i("Placing rooms in dungeon...");
    int i, j;
    int x, y, width, height, room_valid = 1;
    int overlap_valid = 1;
    int place_attempts_fail = 0;
    room_size = 6;
    rooms = (room**)calloc(room_size, sizeof(*rooms));
    
    // initally create the 6 rooms and check if overlap is valid.
    logger::d("Generating initial rooms...");
    do {
        for(i = 0; i < room_size; i++) {
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
            rooms[i] = new room(x, y, width, height);
        }
        
        overlap_valid = 1;
        for(i = 0; i < room_size; i++) {
            for(j = 0; j < i; j++) {
                if(rooms[i]->overlap_with(rooms[j])) {
                    overlap_valid = 0;
                    break;
                }
            }
            if(!overlap_valid) {
                // cleanup rooms, we don't have a valid room series
                for(j = 0; j < room_size; j++) {
                    delete rooms[j];
                }
                break;
            }
        }
    } while(!overlap_valid);
    logger::d("Initial Rooms Generated");
    
    // Check if we have open space and add rooms one at a time
    logger::d("Checking on adding more rooms");
    while(is_open_space() && place_attempts_fail < 2000) {
        logger::d("Adding one more room");
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
        
        room* new_room = new room(x, y, width, height);
        
        logger::d("Room defined: (%2d, %2d, %2d, %2d), checking overlap",
                 new_room->location->x,
                 new_room->location->y,
                 new_room->width,
                 new_room->height);
        overlap_valid = 1;
        for(i = 0; i < room_size; i++) {
            if(new_room->overlap_with(rooms[i])) {
                overlap_valid = 0;
                break;
            }
        }
        if(!overlap_valid) {
            place_attempts_fail++;
            delete new_room;
            logger::d("Overlap Detected, Removing room and trying again");
        } else {
            logger::t("Room is valid, reallocating room array and adding room...");
            room** new_room_array = (room**) realloc(rooms, (room_size + 1) * sizeof(*rooms));
            if(new_room_array == NULL) {
                logger::e("Room Array Reallocation failed!");
                break;
            }
            rooms = new_room_array;
            rooms[room_size++] = new_room;
            logger::t("Room added");
        }
    }
    
    if(place_attempts_fail >= 2000) {
        logger::d("Stopping room placement due to attempt failures: %d", place_attempts_fail);
    }
    
    add_rooms();
    
    for(i = 0; i < room_size; i++) {
        d_log_room(rooms[i]);
    }
    
    logger::i("Rooms Placed in Dungeon");
}

void dungeon::place_staircases() {
    logger::i("Placing staircases");
    
    point* up = new point(0, 0);
    dungeon::rand_point(this, up);
    tile* upstairs = tiles[up->y][up->x];
    upstairs->update_content(tc_UPSTR);
    
    point* down = new point(0, 0);
    do {
        dungeon::rand_point(this, down);
    } while(up->distance_to(down) == 0);
    
    tile* downstairs = tiles[down->y][down->x];
    downstairs->update_content(tc_DNSTR);
    
    delete up;
    delete down;
    
    logger::i("Staircases placed in dungeon");
}

void dungeon::pathfind() {
    logger::i("Generating Corridors...");
    
    graph* g = corridor::construct(this, 0);
    point src(0, 0);
    point dest(0, 0);
    int num_paths = 0;
    int i;
    for(i = 0; i < room_size - 1; i++) {
        if(rooms[i]->connected && rooms[i+1]->connected) {
            logger::t("Room %d and %d are already connected, moving to next path", i, i+1);
            continue;
        }
        
        src.x = (rand() % rooms[i]->width) + rooms[i]->location->x;
        src.y = (rand() % rooms[i]->height) + rooms[i]->location->y;
        dest.x = (rand() % rooms[i+1]->width) + rooms[i+1]->location->x;
        dest.y = (rand() % rooms[i+1]->height) + rooms[i+1]->location->y;
        logger::d("Room Path %2d: Routing from (%2d, %2d) to (%2d, %2d)", i, src.x, src.y, dest.x, dest.y);
        corridor::pathfind(g, this, &src, &dest);
        rooms[i]->connected = 1;
        rooms[i+1]->connected = 1;
        num_paths++;
    }
    delete g;
    
    // Connect last room to first room
    g = corridor::construct(this, 1);
    
    src.x = (rand() % rooms[room_size - 1]->width) + rooms[room_size - 1]->location->x;
    src.y = (rand() % rooms[room_size - 1]->height) + rooms[room_size - 1]->location->y;
    dest.x = (rand() % rooms[0]->width) + rooms[0]->location->x;
    dest.y = (rand() % rooms[0]->height) + rooms[0]->location->y;
    logger::d("Room Path %2d: Routing from (%2d, %2d) to (%2d, %2d)", i, src.x, src.y, dest.x, dest.y);
    corridor::pathfind(g, this, &src, &dest);
    num_paths++;
    
    delete g;
    
    logger::i("Generated %d paths", num_paths);
    logger::i("Corridors Generated");
}

void dungeon::update_path_hardnesses() {
    int i, j;
    tile* t;
    
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            t = tiles[i][j];
            if(t->content == tc_ROOM || t->content == tc_PATH || t->content == tc_UPSTR || t->content == tc_DNSTR) {
                t->update_hardness(0);
            }
        }
    }
}

void dungeon::write_pgm(const char* file_name, int zone) {
    int i, j;
    FILE* pgm = fopen(file_name, "w+");
    
    fprintf(pgm, "P2 \r\n%d %d \r\n255 \r\n", DUNGEON_WIDTH, DUNGEON_HEIGHT);
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            int hardness = tiles[i][j]->rock_hardness;
            int grey_val = hardness == ROCK_HARD ? 255 :
            hardness == ROCK_MED ? 170 :
            hardness == ROCK_SOFT ? 80 : 0;
            grey_val = zone ? grey_val : hardness;
            fprintf(pgm, "%d ", grey_val);
        }
        fprintf(pgm, "\r\n");
    }
}

void dungeon::accent() {
    logger::t("Spiking Dungeon Out...");
    int i, j;
    tile* t;
    int hardnesses[] = {ROCK_HARD, ROCK_MED, ROCK_SOFT};
    
    for(i = 0; i < 3; i++) {
        for(j = 0; j < POINT_LIMIT; j++) {
            int pos;
            do {
                pos = rand() % (DUNGEON_WIDTH * DUNGEON_HEIGHT);
            } while(tiles[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH]->content != tc_UNSET);
            t = tiles[pos / DUNGEON_WIDTH][pos % DUNGEON_WIDTH];
            t->update_hardness(hardnesses[i]);
            t->update_content(tc_ROCK);
        }
    }
    
    logger::t("Dungeon Spiked Out");
    
    if(env_constants::DEBUG_MODE) {
        logger::t("Writing map to pgm");
        write_pgm("rock_accent_map.pgm", 1);
    }
}

void dungeon::diffuse() {
    logger::t("Diffusing Dungeon...");
    int points_hit = 3 * POINT_LIMIT;
    int i,j;
    int k, x_adj, y_adj, pass_bitmap = 0;
    int hardness;
    int x_coords[] = {0, 1, 0, -1, 1, 1, -1, -1};
    int y_coords[] = {-1, 0, 1, 0, -1, 1, 1, -1};
    tile* t;
    while(points_hit < (DUNGEON_WIDTH * DUNGEON_HEIGHT)) {
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                // Look for i,j where we have already set a value
                if(tiles[i][j]->content == tc_UNSET) {
                    continue;
                }
                hardness = tiles[i][j]->rock_hardness;
                pass_bitmap = 0;
                for(k = 0; k < 8; k++) {
                    // Check if we will be in range;
                    x_adj = j + x_coords[k];
                    y_adj = i + y_coords[k];
                    if(x_adj < 0 || x_adj >= DUNGEON_WIDTH || y_adj < 0 || y_adj >= DUNGEON_HEIGHT) {
                        continue;
                    }
                    // Check if the tile has already been set or has had changes proposed
                    t = tiles[y_adj][x_adj];
                    if(t->content != tc_UNSET ||
                       t->are_changes_proposed()) {
                        continue;
                    }
                    
                    // Check if dependent changes have happened (for diagonals)
                    if((k < 4)                       || // an adjacent spot does not have dependency
                       (k == 4 && pass_bitmap >= 3)  || // diagonal checks
                       (k == 5 && pass_bitmap >= 6)  ||
                       (k == 6 && pass_bitmap >= 12) ||
                       (k == 7 && pass_bitmap >= 9)) {
                        // propose the new changes to the tile
                        t->propose_update_content(tc_ROCK);
                        t->propose_update_hardness(hardness);
                        
                        // update the pass bitmap
                        pass_bitmap |= (1 << k);
                    }
                }
            }
        }
        // commit the tile updates for this pass
        for(i = 0; i < DUNGEON_HEIGHT; i++) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                tile* t = tiles[i][j];
                // changes are different than the content, so we hit a point
                if(t->content != t->change_content) {
                    points_hit++;
                }
                t->commit_updates();
            }
        }
    }
    logger::t("Dungeon Diffused");
    
    if(env_constants::DEBUG_MODE) {
        logger::t("Writing diffuse map out");
        write_pgm("rock_diffuse_map.pgm", 1);
    }
}

void dungeon::smooth() {
    logger::t("Smoothing Dungeon...");
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
                sum += tiles[y][x]->rock_hardness;
                tiles_hit++;
            }
            tiles[i][j]->propose_update_hardness(sum / tiles_hit);
        }
    }
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        for(j = 0; j < DUNGEON_WIDTH; j++) {
            tiles[i][j]->commit_updates();
        }
    }
    
    logger::t("Dungeon Smoothed");
    
    if(env_constants::DEBUG_MODE) {
        logger::t("Writing smooth map out");
        write_pgm("rock_smooth_map.pgm", 0);
    }
}

void dungeon::add_borders() {
    logger::t("Bordering Dungeon...");
    int i, j;
    tile* t;
    for(i = 0; i < DUNGEON_HEIGHT; i++) {
        if(i == 0 || i == DUNGEON_HEIGHT - 1) {
            for(j = 0; j < DUNGEON_WIDTH; j++) {
                t = tiles[i][j];
                t->update_content(tc_BORDER);
                t->update_hardness(ROCK_MAX);
            }
        } else {
            t = tiles[i][0];
            t->update_content(tc_BORDER);
            t->update_hardness(ROCK_MAX);
            
            t = tiles[i][DUNGEON_WIDTH - 1];
            t->update_content(tc_BORDER);
            t->update_hardness(ROCK_MAX);
        }
    }
    
    logger::t("Dungeon Bordered");
    
    if(env_constants::DEBUG_MODE) {
        logger::t("Writing terrain map out");
        write_pgm("rock_terrain_map.pgm", 0);
    }
}

bool dungeon::is_open_space() {
    logger::t("Checking Open Space...");
    int i;
    int total_size = 0;
    for(i = 0; i < room_size; i++) {
        total_size += (rooms[i]->height * rooms[i]->width);
    }
    logger::t("Room Space taken: %d out of total space %d", total_size, DUNGEON_WIDTH * DUNGEON_HEIGHT);
    return total_size < (DUNGEON_HEIGHT * DUNGEON_WIDTH * 0.20f);
}

void dungeon::add_rooms() {
    int i, j, k;
    // Rooms are created, add them to _dungeon_array
    for(i = 0; i < room_size; i++) {
        for(j = rooms[i]->location->y; j < rooms[i]->location->y + rooms[i]->height; j++) {
            for(k = rooms[i]->location->x; k < rooms[i]->location->x + rooms[i]->width; k++) {
                tiles[j][k]->update_content(tc_ROOM);
            }
        }
    }
}
