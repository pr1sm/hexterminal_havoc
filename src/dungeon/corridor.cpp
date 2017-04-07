//
//  corridor.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>


#include "corridor.h"
#include "dungeon.h"
#include "../pathing/pathing.h"
#include "../graph/graph.h"
#include "../tile/tile.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../util/util.h"

void corridor::check_room_intercept(dungeon* d, point* point) {
    if(d->room_size < 0) {
        logger::w("Check room called before rooms have been generated!");
        return;
    }
    room* r;
    int i;
    for(i = 0; i < d->room_size; i++) {
        r = d->rooms[i];
        if(r->contains(point) && !r->connected) {
            r->connected = 1;
            logger::t("Point (%d, %d) connects room %d", point->x, point->y, i);
            return;
        }
    }
}

graph* corridor::construct(dungeon* d, int invert) {
    logger::d("Constructing Graph for pathfinding...");
    graph* g = new graph();
    
    // add all edges to graph
    // just use adjacent tiles for now
    int i, j, k, x, y;
    int coord_adj[] = {-1, 0, 1, 0, -1};
    
    for(i = 1; i < DUNGEON_HEIGHT - 1; i++) {
        for(j = 1; j < DUNGEON_WIDTH - 1; j++) {
            tile* t = d->tiles[i][j];
            for(k = 0; k < 4; k++) {
                // check if we are in range
                y = i + coord_adj[k];
                x = j + coord_adj[k + 1];
                if(x < 1 || x >= DUNGEON_WIDTH - 1 || y < 1 || y >= DUNGEON_HEIGHT - 1) {
                    continue;
                }
                tile* dest = d->tiles[y][x];
                
                int weight = invert ? ROCK_MAX-dest->rock_hardness : dest->rock_hardness;
                g->add_edge(t->location, dest->location, weight);
            }
        }
    }
    
    logger::d("Final Vertex Count: %d", g->len);
    logger::d("Final Edge Count: %d", g->edge_count);
    logger::d("Graph for pathfinding constructed");
    return g;
}

void corridor::destruct(graph* g) {
    logger::d("Destructing graph for pathfinding...");
    delete g;
    logger::d("Graph for pathfinding destructed");
}

void corridor::pathfind(graph* g, dungeon* d, point* start, point* end) {
    pathing::dijkstra(g, start, end);
    place_path(g, d, end);
}

void corridor::place_path(graph* g, dungeon* d, point* b) {
    int n;
    vertex* v;
    tile* tile;
    point p(0, 0);
    v = g->vertices[util::point_to_index(b)];
    if(v->dist == INT_MAX) {
        logger::e("no path! exiting program");
        exit(1);
    }
    for(n = 1; v->dist; n++) {
        util::index_to_point(v->index, &p);
        
        tile = d->tiles[p.y][p.x];
        if(tile->content == tc_ROCK) {
            tile->update_content(tc_PATH);
        } else if(tile->content == tc_ROOM) {
            check_room_intercept(d, &p);
        }
        v = g->vertices[v->prev];
        if(v == NULL) break;
    }
}
