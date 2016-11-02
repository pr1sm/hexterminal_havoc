//
//  ai.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
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
#include "../pathing/pathing.h"
#include "../util/util.h"
#include "character.h"

graph* ai::PLAYER_PATH = NULL;

void ai::setup_pc_movement() {
    if(PLAYER_PATH == NULL) {
        PLAYER_PATH = pathfinder::construct(dungeon::get_dungeon(), 0);
    }
    point rand_dest(0, 0);
    character* pc = character::get_pc();
    dungeon::rand_point(dungeon::get_dungeon(), &rand_dest);
    point* pc_pos;
    point* pc_dest;
    pc->set_destination(&rand_dest);
    pc_dest = pc->_destination;
    pc_pos = pc->_position;
    logger::i("Setting player path from (%2d, %2d) to (%2d, %2d)",
             pc_pos->x,
             pc_dest->y,
             rand_dest.x,
             rand_dest.y);
    pathing::dijkstra(PLAYER_PATH, pc_dest, pc_pos);
    event_queue::add_event(pc);
}

void ai::handle_pc_move() {
    character** characters = character_store::get_characters();
    character* pc = character::get_pc();
    point* pc_pos;
    point* pc_dest;
    pc_pos = pc->_position;
    pc_dest = pc->_destination;
    vertex* v = PLAYER_PATH->vertices[util::point_to_index(pc_pos)];
    point next(0, 0);
    util::index_to_point(v->prev, &next);
    pc->set_position(&next);
    pc_pos = pc->_position;
    int i;
    for(i = 0; i < character_store::CHARACTER_COUNT; i++) {
        point* npc_pos = characters[i]->_position;
        if(pc_pos->distance_to(npc_pos) == 0) {
            characters[i]->_is_dead = 1;
        }
    }
    if(pc_pos->distance_to(pc_dest) == 0) {
        setup_pc_movement();
    } else {
        event_queue::add_event(pc);
    }
}

void ai::handle_npc_move(character* c) {
    character_type c_type;
    uint8_t c_is_dead = 0;
    point* c_pos;
    point* c_dest;
    uint8_t c_attrs;
    
    character* pc = character::get_pc();
    point* pc_pos;
    c_type    = c->_type;
    c_is_dead = c->_is_dead;
    c_pos     = c->_position;
    c_attrs   = c->_attrs;
    c_dest    = c->_destination;
    pc_pos    = pc->_position;
    if(c_type == PC) {
        return;
    }
    if(c_is_dead) {
        return;
    }
    int behave_erratically = rand() & 1;
    int dungeon_updated = 0;
    path_node* los_path = los_to_pc(c_pos);
    
    // Setup destination
    if(c_attrs & TELEP_VAL) {
        c->set_destination(pc_pos);
    } else {
        // Check line of sight
        if(los_path != NULL) {
            // path found, update destination in all cases
            c->set_destination(pc_pos);
        } else if(!(c_attrs & INTEL_VAL)) {
            // npc is not intelligent -- destruct destination and set it to NULL
            if(c_dest != NULL && (c_dest->distance_to(pc_pos) == 0)) {
                c->set_destination(NULL);
            }
        }
    }
    
    // refresh c_dest
    c_dest = c->_destination;
    
    dungeon* d = dungeon::get_dungeon();
    
    // Turn Logic
    if((c_attrs & ERATC_VAL) && behave_erratically) {
        // Do random move
        tile* tile = NULL;
        point random_dir(0, 0);
        get_random_dir(c, &random_dir);
        tile = d->tiles[random_dir.y][random_dir.x];
        
        // Move to random position
        if((c_attrs & TUNNL_VAL) && tile->content == tc_ROCK) {
            // Tunnel to position
            uint8_t new_hardness = tile->npc_tunnel();
            dungeon_updated = 1;
            if(new_hardness == 0) {
                // tile is now a path, move immediately
                tile->update_content(tc_PATH);
                c->set_position(tile->location);
            }
        } else {
            // move to position
            c->set_position(tile->location);
        }
    } else {
        // Do normal move
        if(c_attrs & INTEL_VAL) {
            // use path maps
            if(c_dest == NULL || c_dest->distance_to(c_pos) == 0) {
                // pick new point to go to
                point rand_dest(0, 0);
                dungeon::rand_point(d, &rand_dest);
                c->set_destination(&rand_dest);
                // refresh c_dest
                c_dest = c->_destination;
            }
            // choose correct map and use dijkstras
            graph* map = (c_attrs & TUNNL_VAL) ? d->tunnel_map : d->non_tunnel_map;
            pathing::dijkstra(map, c_dest, c_pos);
            vertex* v = map->vertices[util::point_to_index(c_pos)];
            // pathing failed, do nothing!
            if(v != NULL) {
                point next(0, 0);
                util::index_to_point(v->prev, &next);
                tile* tile = d->tiles[next.y][next.x];
                if((c_attrs & TUNNL_VAL) && tile->content == tc_ROCK) {
                    // Tunnel to position
                    uint8_t new_hardness = tile->npc_tunnel();
                    dungeon_updated = 1;
                    if(new_hardness == 0) {
                        // tile is now a path, move immediately
                        tile->update_content(tc_PATH);
                        c->set_position(tile->location);
                    }
                } else {
                    // move to position
                    c->set_position(tile->location);
                }
            }
        } else {
            // use LOS path
            if(los_path != NULL && los_path->next != NULL) {
                // use next path in los_path;
                point* next_pos = los_path->next->curr;
                tile* next_tile = d->tiles[next_pos->y][next_pos->x];
                if(next_tile->content != tc_ROCK) {
                    // next tile is a room/path, move to that position
                    c->set_position(next_tile->location);
                } else if(c_attrs & TUNNL_VAL) {
                    // next tile is rock and npc is a tunneler, tunnel to that position
                    uint8_t new_hardness = next_tile->npc_tunnel();
                    dungeon_updated = 1;
                    if(new_hardness == 0) {
                        // tile is now a path, move immediately
                        next_tile->update_content(tc_PATH);
                        c->set_position(next_tile->location);
                    }
                }
                // next tile is rock and npc is non-tunneler, nothing can be done
            } else {
                // no los, pick a random point to go to
                point p(0, 0);
                dungeon::rand_point(d, &p);
                path_node* new_path = pathing::bresenham(&p, c_pos);
                if(new_path != NULL && new_path->next != NULL) {
                    point* next_pos = new_path->next->curr;
                    tile* next_tile = d->tiles[next_pos->y][next_pos->x];
                    if(next_tile->content != tc_ROCK) {
                        // next tile is a room/path, move to that position
                        c->set_position(next_tile->location);
                    } else if(c_attrs & TUNNL_VAL) {
                        // next tile is rock and npc is a tunneler, tunnel to that position
                        uint8_t new_hardness = next_tile->npc_tunnel();
                        dungeon_updated = 1;
                        if(new_hardness == 0) {
                            // tile is now a path, move immediately
                            next_tile->update_content(tc_PATH);
                            c->set_position(next_tile->location);
                        }
                    }
                    // next tile is rock and npc is non-tunneler, nothing can be done
                }
                if(new_path != NULL) {
                    // Maybe?
                    delete new_path;
                }
                // path couldn't be found, do nothing
            }
        }
    }
    if(los_path != NULL) {
//        graphAPI.destruct_path(los_path);
        delete los_path;
    }
    
    if(dungeon_updated) {
        d->update_path_maps();
    }
    
    event_queue::add_event(c);
}

path_node* ai::los_to_pc(point* p) {
    if(p == NULL) {
        logger::w("los_to_pc called with NULL p, returning NULL!");
        return NULL;
    }
    character* pc = character::get_pc();
    point* pc_pos = pc->_position;
    path_node* path = pathing::bresenham(pc_pos, p);
    path_node* temp = path;
    dungeon* d = dungeon::get_dungeon();
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
        delete path;
        return NULL;
    }
}

void ai::get_random_dir(character* c, point* new_pos) {
    dungeon* d = dungeon::get_dungeon();
    tile* tile;
    point* c_pos;
    uint8_t c_attrs;
    c_pos = c->_position;
    c_attrs = c->_attrs;
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
