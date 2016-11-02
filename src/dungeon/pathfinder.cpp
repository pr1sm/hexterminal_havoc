//
//  pathfinder.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>
#include <cstddef>

#include "pathfinder.h"
#include "dungeon.h"
#include "../pathing/pathing.h"
#include "../graph/graph.h"
#include "../point/point.h"
#include "../tile/tile.h"
#include "../logger/logger.h"
#include "../util/util.h"

int pathfinder::hardness_to_weight(int hardness) {
    if(hardness >= 255) {
        logger::w("Shouldn't convert hardness 255 to weight! defaulting to INT_MAX");
        return INT_MAX;
    }
    return hardness == 0        ? 1 :
           hardness < ROCK_MED  ? 1 :
           hardness < ROCK_HARD ? 2 : 3;
}

graph* pathfinder::construct(dungeon* d, int tunnel) {
    logger::d("Constructing Graph for path mapping%s...", tunnel ? " with tunnelling" : "");
    graph* g = new graph();
    
    // add all edges to graph
    // just use adjacent tiles for now
    int i, j, x, y;
    
    for(i = 1; i < DUNGEON_HEIGHT - 1; i++) {
        for(j = 1; j < DUNGEON_WIDTH - 1; j++) {
            tile* t = d->tiles[i][j];
            if(!tunnel && t->content == tc_ROCK) {
                continue;
            }
            for(x = j-1; x < j+2; x++) {
                for(y = i-1; y < i+2; y++) {
                    // check if we are in bounds
                    if(x < 1 || x >= DUNGEON_WIDTH - 1 || y < 1 || y >= DUNGEON_HEIGHT - 1) {
                        continue;
                    }
                    tile* dest = d->tiles[y][x];
                    // check if we are tunnelling or not
                    if(!tunnel && dest->content == tc_ROCK) {
                        continue;
                    }
                    
                    int weight = tunnel ? hardness_to_weight(t->rock_hardness) : 1;
                    
                    g->add_edge(t->location, dest->location, weight);
                }
            }
        }
    }
    
    logger::d("Final Vertex Count: %d", g->len);
    logger::d("Final Edge Count: %d", g->edge_count);
    logger::d("Graph for path mapping constructed");
    return g;
}

void pathfinder::destruct(graph* g) {
    logger::d("Destructing graph for path mapping...");
    delete g;
    logger::d("Graph for path mapping destructed");
}

int pathfinder::generate_pathmap(graph* g, dungeon* d, point* start, int tunnel) {
    logger::i("Generating path map%s...", tunnel ? " with tunnelling" : "");
    int error = pathing::dijkstra(g, start, nullptr);
    update_tiles(g, d, tunnel);
    logger::i("Path map Generated");
    return error;
}

void pathfinder::update_tiles(graph* g, dungeon* d, int tunnel) {
    int i;
    for(i = 0; i < g->size; i++) {
        vertex* v = g->vertices[i];
        if(v == nullptr) {
            continue;
        }
        point p(0, 0);
        util::index_to_point(v->index, &p);
        tile* t = d->tiles[p.y][p.x];
        if(tunnel) {
            t->update_dist_tunnel(v->dist);
        } else {
            t->update_dist(v->dist);
        }
    }
}
