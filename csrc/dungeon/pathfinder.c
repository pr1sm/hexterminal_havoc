//
//  pathfinder.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>

#include "pathfinder.h"
#include "dungeon.h"
#include "../dijkstra/dijkstra.h"
#include "../graph/graph.h"
#include "../point/point.h"
#include "../tile/tile.h"
#include "../logger/logger.h"
#include "../util/util.h"

static void update_tiles(graph_t* g, dungeon_t* d, int tunnel);

static int hardness_to_weight(int hardness) {
    if(hardness >= 255) {
        logger.w("Shouldn't convert hardness 255 to weight! defaulting to INT_MAX");
        return INT_MAX;
    }
    return hardness == 0        ? 1 :
           hardness < ROCK_MED  ? 1 :
           hardness < ROCK_HARD ? 2 : 3;
}

static graph_t* construct(dungeon_t* d, int tunnel) {
    logger.d("Constructing Graph for path mapping%s...", tunnel ? " with tunnelling" : "");
    graph_t* g = (graph_t*)calloc(1, sizeof(graph_t));
    g->point_to_index = point_to_index;
    
    // add all edges to graph
    // just use adjacent tiles for now
    int i, j, x, y;
    
    for(i = 1; i < DUNGEON_HEIGHT - 1; i++) {
        for(j = 1; j < DUNGEON_WIDTH - 1; j++) {
            tile_t* t = d->tiles[i][j];
            if(!tunnel && t->content == tc_ROCK) {
                continue;
            }
            for(x = j-1; x < j+2; x++) {
                for(y = i-1; y < i+2; y++) {
                    // check if we are in bounds
                    if(x < 1 || x >= DUNGEON_WIDTH - 1 || y < 1 || y >= DUNGEON_HEIGHT - 1) {
                        continue;
                    }
                    tile_t* dest = d->tiles[y][x];
                    // check if we are tunnelling or not
                    if(!tunnel && dest->content == tc_ROCK) {
                        continue;
                    }
                    
                    int weight = tunnel ? hardness_to_weight(t->rock_hardness) : 1;
                    
                    graphAPI.add_edge(g, t->location, dest->location, weight);
                }
            }
        }
    }
    
    logger.d("Final Vertex Count: %d", g->len);
    logger.d("Final Edge Count: %d", g->edge_count);
    logger.d("Graph for path mapping constructed");
    return g;
}

static void destruct(graph_t* g) {
    logger.d("Destructing graph for path mapping...");
    int i;
    for(i = 0; i < g->size; i++) {
        if(g->vertices[i] != NULL) {
            graphAPI.free_vertex(g->vertices[i]);
        }
    }
    free(g->vertices);
    free(g);
    logger.d("Graph for path mapping destructed");
}

static int gen_map(graph_t* g, dungeon_t* d, point_t* start, int tunnel) {
    logger.i("Generating path map%s...", tunnel ? " with tunnelling" : "");
    int error = dijkstraAPI.dijkstra(g, start, NULL);
    update_tiles(g, d, tunnel);
    logger.i("Path map Generated");
    return error;
}

static void update_tiles(graph_t* g, dungeon_t* d, int tunnel) {
    int i;
    for(i = 0; i < g->size; i++) {
        vertex_t* v = g->vertices[i];
        if(v == NULL) {
            continue;
        }
        point_t p;
        index_to_point(v->index, &p);
        tile_t* t = d->tiles[p.y][p.x];
        if(tunnel) {
            t->update_dist_tunnel(t, v->dist);
        } else {
            t->update_dist(t, v->dist);
        }
    }
}

pathfinder_namespace const pathfinderAPI = {
    construct,
    destruct,
    gen_map
};
