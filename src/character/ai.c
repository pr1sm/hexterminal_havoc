//
//  ai.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/16/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "ai.h"
#include "character_store.h"
#include "../events/event_queue.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../graph/graph.h"
#include "../dungeon/pathfinder.h"
#include "../dijkstra/dijkstra.h"
#include "../util/util.h"
#include "character.h"
#ifdef __cplusplus
#else
#include "character_t.h"
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

graph_t* _PLAYER_PATH = NULL;

static void get_random_dir(character_t* c, point_t* new_pos);
static void set_destination(character_t* c, point_t* dest);
static void set_position(character_t* c, point_t* pos);

void setup_pc_movement() {
    if(_PLAYER_PATH == NULL) {
        _PLAYER_PATH = pathfinderAPI.construct(dungeonAPI.get_dungeon(), 0);
    }
    point_t rand_dest;
    character_t* pc = characterAPI.get_pc();
    dungeonAPI.rand_point(dungeonAPI.get_dungeon(), &rand_dest);
    point_t* pc_pos;
    point_t* pc_dest;
#ifdef __cplusplus
    characterAPI.set_dest(pc, &rand_dest);
    pc_dest = characterAPI.get_dest(pc);
    pc_pos = characterAPI.get_pos(pc);
#else
    pc->set_destination(pc, &rand_dest);
    pc_dest = pc->destination;
    pc_pos = pc->position;
#endif // __cplusplus
    logger.i("Setting player path from (%2d, %2d) to (%2d, %2d)",
             pc_pos->x,
             pc_dest->y,
             rand_dest.x,
             rand_dest.y);
    dijkstraAPI.dijkstra(_PLAYER_PATH, pc_dest, pc_pos);
    eventQueueAPI.add_event(pc);
}

void handle_pc_move() {
    character_t** characters = characterStoreAPI.get_characters();
    character_t* pc = characterAPI.get_pc();
    point_t* pc_pos;
    point_t* pc_dest;
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(pc);
    pc_dest = characterAPI.get_dest(pc);
#else
    pc_pos = pc->position;
    pc_dest = pc->destination;
#endif // __cplusplus
    vertex_t* v = _PLAYER_PATH->vertices[point_to_index(pc_pos)];
    point_t next;
    index_to_point(v->prev, &next);
#ifdef __cplusplus
    characterAPI.set_pos(pc, &next);
    pc_pos = characterAPI.get_pos(pc);
#else
    pc->set_position(pc, &next);
    pc_pos = pc->position;
#endif // __cplusplus
    int i;
    for(i = 0; i < CHARACTER_COUNT; i++) {
        point_t* npc_pos;
#ifdef __cplusplus
        npc_pos = characterAPI.get_pos(characters[i]);
        if(pc_pos->distance(pc_pos, npc_pos) == 0) {
            characterAPI.set_is_dead(characters[i], 1);
        }
#else
        npc_pos = characters[i]->position;
        if(pc_pos->distance(pc_pos, npc_pos) == 0) {
            characters[i]->is_dead = 1;
        }
#endif // __cplusplus
    }
    if(pc_pos->distance(pc_pos, pc_dest) == 0) {
        setup_pc_movement();
    } else {
        eventQueueAPI.add_event(pc);
    }
}

void handle_npc_move(character_t* c) {
    character_type c_type;
    uint8_t c_is_dead = 0;
    point_t* c_pos;
    point_t* c_dest;
    uint8_t c_attrs;
    
    character_t* pc = characterAPI.get_pc();
    point_t* pc_pos;
    
#ifdef __cplusplus
    c_type    = characterAPI.get_type(c);
    c_is_dead = characterAPI.get_is_dead(c);
    c_pos     = characterAPI.get_pos(c);
    c_attrs   = characterAPI.get_attrs(c);
    c_dest    = characterAPI.get_dest(c);
    pc_pos    = characterAPI.get_pos(pc);
#else
    c_type    = c->type;
    c_is_dead = c->is_dead;
    c_pos     = c->position;
    c_attrs   = c->attrs;
    c_dest    = c->destination;
    pc_pos    = pc->position;
#endif // __cplusplus
    if(c_type == PC) {
        return;
    }
    if(c_is_dead) {
        return;
    }
    int behave_erratically = rand() & 1;
    int dungeon_updated = 0;
    path_node_t* los_path = los_to_pc(c_pos);
    
    // Setup destination
    if(c_attrs & TELEP_VAL) {
        set_destination(c, pc_pos);
    } else {
        // Check line of sight
        if(los_path != NULL) {
            // path found, update destination in all cases
            set_destination(c, pc_pos);
        } else if(!(c_attrs & INTEL_VAL)) {
            // npc is not intelligent -- destruct destination and set it to NULL
            if(c_dest != NULL && (c_dest->distance(c_dest, pc_pos) == 0)) {
                set_destination(c, NULL);
            }
        }
    }
    
    // refresh c_dest
#ifdef __cplusplus
    c_dest    = characterAPI.get_dest(c);
#else
    c_dest = c->destination;
#endif // __cplusplus
    
    dungeon_t* d = dungeonAPI.get_dungeon();
    
    // Turn Logic
    if((c_attrs & ERATC_VAL) && behave_erratically) {
        // Do random move
        tile_t* tile = NULL;
        point_t random_dir;
        get_random_dir(c, &random_dir);
        tile = d->tiles[random_dir.y][random_dir.x];
        
        // Move to random position
        if((c_attrs & TUNNL_VAL) && tile->content == tc_ROCK) {
            // Tunnel to position
            uint8_t new_hardness = tile->npc_tunnel(tile);
            dungeon_updated = 1;
            if(new_hardness == 0) {
                // tile is now a path, move immediately
                tile->update_content(tile, tc_PATH);
                set_position(c, tile->location);
            }
        } else {
            // move to position
            set_position(c, tile->location);
        }
    } else {
        // Do normal move
        if(c_attrs & INTEL_VAL) {
            // use path maps
            if(c_dest == NULL || c_dest->distance(c_dest, c_pos) == 0) {
                // pick new point to go to
                point_t rand_dest;
                dungeonAPI.rand_point(d, &rand_dest);
                set_destination(c, &rand_dest);
                // refresh c_dest
#ifdef __cplusplus
                c_dest    = characterAPI.get_dest(c);
#else
                c_dest = c->destination;
#endif // __cplusplus
            }
            // choose correct map and use dijkstras
            graph_t* map = (c_attrs & TUNNL_VAL) ? d->tunnel_map : d->non_tunnel_map;
            dijkstraAPI.dijkstra(map, c_dest, c_pos);
            vertex_t* v = map->vertices[point_to_index(c_pos)];
            // pathing failed, do nothing!
            if(v != NULL) {
                point_t next;
                index_to_point(v->prev, &next);
                tile_t* tile = d->tiles[next.y][next.x];
                if((c_attrs & TUNNL_VAL) && tile->content == tc_ROCK) {
                    // Tunnel to position
                    uint8_t new_hardness = tile->npc_tunnel(tile);
                    dungeon_updated = 1;
                    if(new_hardness == 0) {
                        // tile is now a path, move immediately
                        tile->update_content(tile, tc_PATH);
                        set_position(c, tile->location);
                    }
                } else {
                    // move to position
                    set_position(c, tile->location);
                }
            }
        } else {
            // use LOS path
            if(los_path != NULL && los_path->next != NULL) {
                // use next path in los_path;
                point_t* next_pos = los_path->next->curr;
                tile_t* next_tile = d->tiles[next_pos->y][next_pos->x];
                if(next_tile->content != tc_ROCK) {
                    // next tile is a room/path, move to that position
                    set_position(c, next_tile->location);
                } else if(c_attrs & TUNNL_VAL) {
                    // next tile is rock and npc is a tunneler, tunnel to that position
                    uint8_t new_hardness = next_tile->npc_tunnel(next_tile);
                    dungeon_updated = 1;
                    if(new_hardness == 0) {
                        // tile is now a path, move immediately
                        next_tile->update_content(next_tile, tc_PATH);
                        set_position(c, next_tile->location);
                    }
                }
                // next tile is rock and npc is non-tunneler, nothing can be done
            } else {
                // no los, pick a random point to go to
                point_t p;
                dungeonAPI.rand_point(d, &p);
                path_node_t* new_path = dijkstraAPI.bresenham(&p, c_pos);
                if(new_path != NULL && new_path->next != NULL) {
                    point_t* next_pos = new_path->next->curr;
                    tile_t* next_tile = d->tiles[next_pos->y][next_pos->x];
                    if(next_tile->content != tc_ROCK) {
                        // next tile is a room/path, move to that position
                        set_position(c, next_tile->location);
                    } else if(c_attrs & TUNNL_VAL) {
                        // next tile is rock and npc is a tunneler, tunnel to that position
                        uint8_t new_hardness = next_tile->npc_tunnel(next_tile);
                        dungeon_updated = 1;
                        if(new_hardness == 0) {
                            // tile is now a path, move immediately
                            next_tile->update_content(next_tile, tc_PATH);
                            set_position(c, next_tile->location);
                        }
                    }
                    // next tile is rock and npc is non-tunneler, nothing can be done
                }
                if(new_path != NULL) {
                    graphAPI.destruct_path(new_path);
                }
                // path couldn't be found, do nothing
            }
        }
    }
    if(los_path != NULL) {
        graphAPI.destruct_path(los_path);
    }
    
    if(dungeon_updated) {
        d->update_path_maps(d);
    }
    
    eventQueueAPI.add_event(c);
}

path_node_t* los_to_pc(point_t* p) {
    if(p == NULL) {
        logger.w("los_to_pc called with NULL p, returning NULL!");
        return NULL;
    }
    character_t* pc = characterAPI.get_pc();
    point_t* pc_pos;
#ifdef __cplusplus
    pc_pos = characterAPI.get_pos(pc);
#else
    pc_pos = pc->position;
#endif // __cplusplus
    path_node_t* path = dijkstraAPI.bresenham(pc_pos, p);
    path_node_t* temp = path;
    dungeon_t* d = dungeonAPI.get_dungeon();
    int has_los = 1;
    while(temp != NULL) {
        if(d->tiles[temp->curr->y][temp->curr->x]->content == tc_ROCK) {
            // found a rock, no line of sight
            has_los = 0;
            break;
        }
        temp = temp->next;
    }
    if(has_los) {
        return path;
    } else {
        graphAPI.destruct_path(path);
        return NULL;
    }
}

static void get_random_dir(character_t* c, point_t* new_pos) {
    dungeon_t* d = dungeonAPI.get_dungeon();
    tile_t* tile;
    point_t* c_pos;
    uint8_t c_attrs;
#ifdef __cplusplus
    c_pos = characterAPI.get_pos(c);
    c_attrs = characterAPI.get_attrs(c);
#else
    c_pos = c->position;
    c_attrs = c->attrs;
#endif // __cplusplus
    int is_valid = 0;
    int new_x = c_pos->x;
    int new_y = c_pos->y;
    do {
        int random_dir = rand() % 8;
        // convert direction to coordinate offset
        int y_off = (random_dir < 3) ? -1 : (random_dir > 3 && random_dir < 7) ? 1 : 0;
        int x_off = (random_dir < 1) ? -1 : (random_dir > 1 && random_dir < 6) ? 1 : 0;
        y_off += c_pos->y;
        x_off += c_pos->x;
        if(x_off > 1 && x_off < DUNGEON_WIDTH-1 && y_off > 1 && y_off < DUNGEON_HEIGHT-1) {
            tile = d->tiles[y_off][x_off];
            if((c_attrs & TUNNL_VAL) || tile->content != tc_ROCK) {
                is_valid = 1;
                new_x = x_off;
                new_y = y_off;
            }
        } else {
            is_valid = 0;
        }
    } while (!is_valid);
    new_pos->x = new_x;
    new_pos->y = new_y;
}

static void set_destination(character_t* c, point_t* dest) {
#ifdef __cplusplus
    characterAPI.set_dest(c, dest);
#else
    c->set_destination(c, dest);
#endif // __cplusplus
}

static void set_position(character_t* c, point_t* pos) {
#ifdef __cplusplus
    characterAPI.set_pos(c, pos);
#else
    c->set_position(c, pos);
#endif // __cplusplus
}
    
#ifdef __cplusplus
}
#endif // __cplusplus

