//
//  character_store.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "character_store.h"
#include "character.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"
#include "../events/event_queue.h"
#include "../env/env.h"

// Temporary for player movement
#include "../graph/graph.h"
#include "../dungeon/pathfinder.h"
#include "../dijkstra/dijkstra.h"


static character_t** _characters = NULL;
static int _characters_size = 0;
static int _characters_count = 0;

// Temporary for player movement!
static graph_t* player_path = NULL;
static void temp_setup_player_movement();
path_node_t* temp_has_los_to_pc(point_t* p);

static void setup_npc(character_t* npc);

static void print_char(character_t* npc) {
    if(DEBUG_MODE) {
        printf("NPC: spawn: (%2d, %2d) ",
               npc->position->x,
               npc->position->y);
        if(npc->destination != NULL) {
            printf("dest: (%2d, %2d) ",
                   npc->destination->x,
                   npc->destination->y);
        } else {
            printf("dest: NONE       ");
        }
        printf("attrs: %x\n", npc->attrs);
    }
    logger.d("NPC: spawn: (%2d, %2d) ",
           npc->position->x,
           npc->position->y);
    if(npc->destination != NULL) {
        logger.d("dest: (%2d, %2d) ",
               npc->destination->x,
               npc->destination->y);
    } else {
        logger.d("dest: NONE     ");
    }
    logger.d("attrs: %x\n", npc->attrs);
}

static void setup_impl() {
    int nummon = NUM_MONSTERS;
    int i;
    dungeon_t* d = dungeonAPI.get_dungeon();
    _characters_size = nummon + 1; // num of monsters + pc
    _characters = (character_t**) calloc(_characters_size, sizeof(*_characters));
    _characters_count = _characters_size;
    _characters[0] = characterAPI.get_pc();
    point_t* pc_pos = _characters[0]->position;
    temp_setup_player_movement();
    for(i = 1; i < _characters_size; i++) {
        point_t* spawn = pointAPI.construct(0, 0);
        do {
            dungeonAPI.rand_point(d, spawn);
        } while(spawn->distance(spawn, pc_pos) == 0);
        
        character_t* npc = characterAPI.construct_npc(spawn);
        
        setup_npc(npc);
        
        eventQueueAPI.add_event(npc, MOVE);
        _characters[i] = npc;
        
        free(spawn);
        print_char(npc);
    }
}

static void teardown_impl() {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(_characters[i] != NULL) {
            characterAPI.destruct(_characters[i]);
        }
    }
    free(_characters);
}

static int contains_npc_impl(point_t* p) {
    int i;
    // check npcs first, then player
    for(i = 1; i < _characters_count; i++) {
        if(p->distance(p, _characters[i]->position) == 0 && !_characters[i]->is_dead) {
            return i;
        }
    }
    if(p->distance(p, _characters[0]->position) == 0) {
        return 0;
    }
    return -1;
}

static char get_char_for_npc_at_index_impl(int i) {
    return characterAPI.char_for_npc_type(_characters[i]);
}

static void setup_npc(character_t* npc) {
    point_t* pc_pos = _characters[0]->position;
    // telepathic npcs get the pc position
    if(npc->attrs & TELEP_VAL) {
        if(npc->destination == NULL) {
            npc->destination = pointAPI.construct(pc_pos->x, pc_pos->y);
        } else {
            npc->destination->x = pc_pos->x;
            npc->destination->y = pc_pos->y;
        }
    } else {
        // check los on other npcs
        path_node_t* los_path = temp_has_los_to_pc(npc->position);
        if(los_path != NULL) {
            if(npc->destination == NULL) {
                npc->destination = pointAPI.construct(pc_pos->x, pc_pos->y);
            } else {
                npc->destination->x = pc_pos->x;
                npc->destination->y = pc_pos->y;
            }
            graphAPI.destruct_path(los_path);
        }
    }
}


// Temp for player movement!
static void temp_setup_player_movement() {
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
    eventQueueAPI.add_event(characterAPI.get_pc(), MOVE);
}

static int point_to_index(point_t* p) {
    // since outer rows and cols aren't being used
    // subtract one from both so the index starts at 0
    return ((p->y - 1) * (DUNGEON_WIDTH-2)) + (p->x - 1);
}

static void index_to_point(int index, point_t* p) {
    p->x = (index % (DUNGEON_WIDTH-2))+1;
    p->y = (index / (DUNGEON_WIDTH-2))+1;
}

void temp_handle_player_move() {
    character_t* pc = characterAPI.get_pc();
    vertex_t* v = player_path->vertices[point_to_index(pc->position)];
    point_t next;
    index_to_point(v->prev, &next);
    pc->set_position(pc, &next);
    int i;
    for(i = 1; i < _characters_count; i++) {
        if(pc->position->distance(pc->position, _characters[i]->position) == 0) {
            _characters[i]->is_dead = 1;
        }
    }
    if(pc->position->distance(pc->position, pc->destination) == 0) {
        temp_setup_player_movement();
    } else {
        eventQueueAPI.add_event(pc, MOVE);
    }
}

int temp_is_finished() {
    int i;
    point_t* pc_pos = characterAPI.get_pc()->position;
    // check all npcs that ARE NOT the pc for collision
    int win_count = 0;
    for(i = 1; i < _characters_count; i++) {
        if(_characters[i]->is_dead) {
            win_count++;
        }
        if(pc_pos->distance(pc_pos, _characters[i]->position) == 0 && !_characters[i]->is_dead) {
            return 1;
        }
    }
    if(win_count >= _characters_count-1) {
        return 2;
    }
    return 0;
}

path_node_t* temp_has_los_to_pc(point_t* p) {
    // TODO: VALIDATE INPUTS
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

void temp_handle_npc_turn(character_t* c) {
    if(c->type == PC) {
        return;
    }
    if(c->is_dead) {
        return;
    }
    int behave_erratically = rand() & 1;
    int dungeon_updated = 0;
    path_node_t* los_path = temp_has_los_to_pc(c->position);
    
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
        int is_valid = 0;
        // Do random move
        tile_t* tile = NULL;
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
                }
            } else {
                is_valid = 0;
            }
        } while (!is_valid);
        
        // Move to random position
        if((c->attrs & TUNNL_VAL) && tile->content == tc_ROCK) {
            // Tunnel to position
            uint8_t new_hardness = tile->rock_hardness - 85 > 0 ? tile->rock_hardness - 85 : 0;
            tile->update_hardness(tile, new_hardness);
            dungeon_updated = 1;
            if(new_hardness == 0) {
                // tile is now a path, move immediately
                tile->update_content(tile, tc_PATH);
                c->position->x = tile->location->x;
                c->position->y = tile->location->y;
            }
        } else {
            // move to position
            c->position->x = tile->location->x;
            c->position->y = tile->location->y;
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
                    uint8_t new_hardness = tile->rock_hardness - 85 > 0 ? tile->rock_hardness - 85 : 0;
                    tile->update_hardness(tile, new_hardness);
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
            if(los_path != NULL) {
                // use next path in los_path;
                point_t* next_pos = los_path->next->curr;
                tile_t* next_tile = d->tiles[next_pos->y][next_pos->x];
                if(next_tile->content != tc_ROCK) {
                    // next tile is a room/path, move to that position
                    c->set_position(c, next_tile->location);
                } else if(c->attrs & TUNNL_VAL) {
                    // next tile is rock and npc is a tunneler, tunnel to that position
                    uint8_t new_hardness = next_tile->rock_hardness - 85 > 0 ? next_tile->rock_hardness - 85 : 0;
                    next_tile->update_hardness(next_tile, new_hardness);
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
                if(new_path != NULL) {
                    point_t* next_pos = new_path->next->curr;
                    tile_t* next_tile = d->tiles[next_pos->y][next_pos->x];
                    if(next_tile->content != tc_ROCK) {
                        // next tile is a room/path, move to that position
                        c->set_position(c, next_tile->location);
                    } else if(c->attrs & TUNNL_VAL) {
                        // next tile is rock and npc is a tunneler, tunnel to that position
                        uint8_t new_hardness = next_tile->rock_hardness - 85 > 0 ? next_tile->rock_hardness - 85 : 0;
                        next_tile->update_hardness(next_tile, new_hardness);
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
    
    eventQueueAPI.add_event(c, MOVE);
}

character_store_namespace const characterStoreAPI = {
    setup_impl,
    teardown_impl,
    contains_npc_impl,
    get_char_for_npc_at_index_impl
};
