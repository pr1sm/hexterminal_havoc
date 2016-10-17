//
//  pc_ai.c
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

static graph_t* player_path = NULL;

static void get_random_dir(character_t* c, point_t* new_pos);

void setup_pc_movement() {
    if(player_path == NULL) {
        player_path = pathfinderAPI.construct(dungeonAPI.get_dungeon(), 0);
    }
    point_t rand_dest;
    character_t* pc = characterAPI.get_pc();
    dungeonAPI.rand_point(dungeonAPI.get_dungeon(), &rand_dest);
    pc->set_destination(pc, &rand_dest);
    logger.i("Setting player path from (%2d, %2d) to (%2d, %2d)",
             characterAPI.get_pc()->position->x,
             characterAPI.get_pc()->position->y,
             rand_dest.x,
             rand_dest.y);
    dijkstraAPI.dijkstra(player_path, characterAPI.get_pc()->destination, characterAPI.get_pc()->position);
    eventQueueAPI.add_event(characterAPI.get_pc());
}

void handle_pc_move() {
    character_t** characters = characterStoreAPI.get_characters();
    character_t* pc = characterAPI.get_pc();
    vertex_t* v = player_path->vertices[point_to_index(pc->position)];
    point_t next;
    index_to_point(v->prev, &next);
    pc->set_position(pc, &next);
    int i;
    for(i = 1; i < CHARACTER_COUNT; i++) {
        if(pc->position->distance(pc->position, characters[i]->position) == 0) {
            characters[i]->is_dead = 1;
        }
    }
    if(pc->position->distance(pc->position, pc->destination) == 0) {
        setup_pc_movement();
    } else {
        eventQueueAPI.add_event(pc);
    }
}

void handle_npc_move(character_t* c) {
    if(c->type == PC) {
        return;
    }
    if(c->is_dead) {
        return;
    }
    int behave_erratically = rand() & 1;
    int dungeon_updated = 0;
    path_node_t* los_path = los_to_pc(c->position);
    
    // Setup destination
    point_t* pc_pos = characterAPI.get_pc()->position;
    if(c->attrs & TELEP_VAL) {
        c->set_destination(c, pc_pos);
    } else {
        // Check line of sight
        if(los_path != NULL) {
            // path found, update destination in all cases
            c->set_destination(c, pc_pos);
        } else if(!(c->attrs & INTEL_VAL)) {
            // npc is not intelligent -- destruct destination and set it to NULL
            if(c->destination != NULL && (c->destination->distance(c->destination, pc_pos) == 0)) {
                pointAPI.destruct(c->destination);
                c->destination = NULL;
            }
        }
    }
    
    dungeon_t* d = dungeonAPI.get_dungeon();
    
    // Turn Logic
    if((c->attrs & ERATC_VAL) && behave_erratically) {
        // Do random move
        tile_t* tile = NULL;
        point_t random_dir;
        get_random_dir(c, &random_dir);
        tile = d->tiles[random_dir.y][random_dir.x];
        
        // Move to random position
        if((c->attrs & TUNNL_VAL) && tile->content == tc_ROCK) {
            // Tunnel to position
            uint8_t new_hardness = tile->npc_tunnel(tile);
            dungeon_updated = 1;
            if(new_hardness == 0) {
                // tile is now a path, move immediately
                tile->update_content(tile, tc_PATH);
                c->set_position(c, tile->location);
            }
        } else {
            // move to position
            c->set_position(c, tile->location);
        }
    } else {
        // Do normal move
        if(c->attrs & INTEL_VAL) {
            // use path maps
            if(c->destination == NULL || c->destination->distance(c->destination, c->position) == 0) {
                // pick new point to go to
                point_t rand_dest;
                dungeonAPI.rand_point(d, &rand_dest);
                c->set_destination(c, &rand_dest);
            }
            // choose correct map and use dijkstras
            graph_t* map = (c->attrs & TUNNL_VAL) ? d->tunnel_map : d->non_tunnel_map;
            dijkstraAPI.dijkstra(map, c->destination, c->position);
            vertex_t* v = map->vertices[point_to_index(c->position)];
            // pathing failed, do nothing!
            if(v != NULL) {
                point_t next;
                index_to_point(v->prev, &next);
                tile_t* tile = d->tiles[next.y][next.x];
                if((c->attrs & TUNNL_VAL) && tile->content == tc_ROCK) {
                    // Tunnel to position
                    uint8_t new_hardness = tile->npc_tunnel(tile);
                    dungeon_updated = 1;
                    if(new_hardness == 0) {
                        // tile is now a path, move immediately
                        tile->update_content(tile, tc_PATH);
                        c->set_position(c, tile->location);
                    }
                } else {
                    // move to position
                    c->set_position(c, tile->location);
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
                    c->set_position(c, next_tile->location);
                } else if(c->attrs & TUNNL_VAL) {
                    // next tile is rock and npc is a tunneler, tunnel to that position
                    uint8_t new_hardness = next_tile->npc_tunnel(next_tile);
                    dungeon_updated = 1;
                    if(new_hardness == 0) {
                        // tile is now a path, move immediately
                        next_tile->update_content(next_tile, tc_PATH);
                        c->set_position(c, next_tile->location);
                    }
                }
                // next tile is rock and npc is non-tunneler, nothing can be done
            } else {
                // no los, pick a random point to go to
                point_t p;
                dungeonAPI.rand_point(d, &p);
                path_node_t* new_path = dijkstraAPI.bresenham(&p, c->position);
                if(new_path != NULL && new_path->next != NULL) {
                    point_t* next_pos = new_path->next->curr;
                    tile_t* next_tile = d->tiles[next_pos->y][next_pos->x];
                    if(next_tile->content != tc_ROCK) {
                        // next tile is a room/path, move to that position
                        c->set_position(c, next_tile->location);
                    } else if(c->attrs & TUNNL_VAL) {
                        // next tile is rock and npc is a tunneler, tunnel to that position
                        uint8_t new_hardness = next_tile->npc_tunnel(next_tile);
                        dungeon_updated = 1;
                        if(new_hardness == 0) {
                            // tile is now a path, move immediately
                            next_tile->update_content(next_tile, tc_PATH);
                            c->set_position(c, next_tile->location);
                        }
                    }
                    // next tile is rock and npc is non-tunneler, nothing can be done
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
    point_t* pc_pos = characterAPI.get_pc()->position;
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
    int is_valid = 0;
    int new_x = c->position->x;
    int new_y = c->position->y;
    do {
        int random_dir = rand() % 8;
        // convert direction to coordinate offset
        int y_off = (random_dir < 3) ? -1 : (random_dir > 3 && random_dir < 7) ? 1 : 0;
        int x_off = (random_dir < 1) ? -1 : (random_dir > 1 && random_dir < 6) ? 1 : 0;
        y_off += c->position->y;
        x_off += c->position->x;
        if(x_off > 1 && x_off < DUNGEON_WIDTH-1 && y_off > 1 && y_off < DUNGEON_HEIGHT-1) {
            tile = d->tiles[y_off][x_off];
            if((c->attrs & TUNNL_VAL) || tile->content != tc_ROCK) {
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
