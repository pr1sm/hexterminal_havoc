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

static void path_update_tiles(graph_t* g, int tunnel);

static int path_point_to_index(point_t* p) {
    // since outer rows and cols aren't being used
    // subtract one from both so the index starts at 0
    return ((p->y - 1) * (DUNGEON_WIDTH-2)) + (p->x - 1);
}

static point_t index_to_point(int index) {
    point_t p = {(index % (DUNGEON_WIDTH-2))+1, (index / (DUNGEON_WIDTH-2))+1};
    return p;
}

static int hardness_to_weight(int hardness) {
    if(hardness == 255) {
        logger.w("Shouldn't convert hardness 255 to weight! defaulting to 3");
    }
    return hardness == 0 ? 0 :
           hardness < 85 ? 1 :
           hardness < 171 ? 2 : 3;
}

graph_t* path_construct(int tunnel) {
    logger.d("Constructing Graph for path mapping%s...", tunnel ? " with tunnelling" : "");
    graph_t* g = calloc(1, sizeof(graph_t));
    
    // add all edges to graph
    // just use adjacent tiles for now
    int i, j, x, y;
    
    for(i = 1; i < DUNGEON_HEIGHT - 1; i++) {
        for(j = 1; j < DUNGEON_WIDTH - 1; j++) {
            tile_t* t = _dungeon_array[i][j];
            if(!tunnel && t->content == tc_ROCK) {
                continue;
            }
            for(x = j-1; x < j+2; x++) {
                for(y = i-1; y < i+2; y++) {
                    // check if we are in bounds
                    if(x < 1 || x >= DUNGEON_WIDTH - 1 || y < 1 || y >= DUNGEON_HEIGHT - 1) {
                        continue;
                    }
                    tile_t* dest = _dungeon_array[y][x];
                    // check if we are tunnelling or not
                    if(!tunnel && dest->content == tc_ROCK) {
                        continue;
                    }
                    
                    int weight = 1 + (tunnel ? hardness_to_weight(dest->rock_hardness) : 0);
                    
                    graphAPI.add_edge(g, t->location, dest->location, weight, path_point_to_index);
                }
            }
        }
    }
    
    logger.d("Final Vertex Count: %d", g->len);
    logger.d("Final Edge Count: %d", g->edge_count);
    logger.d("Graph for path mapping constructed");
    return g;
}

void path_destruct(graph_t* g) {
    logger.d("Destructing graph for path mapping...");
    int i;
    for(i = 0; i < g->size; i++) {
        if(g->vertices[i] != NULL) {
            graphAPI.free_vertex(g->vertices[i]);
        }
    }
    free(g);
    logger.d("Graph for path mapping destructed");
}

void path_gen_map(graph_t* g, point_t* start, int tunnel) {
    logger.i("Generating path map%s...", tunnel ? " with tunnelling" : "");
    dijkstraAPI.dijkstra(g, start, NULL, path_point_to_index);
    path_update_tiles(g, tunnel);
    logger.i("Path map Generated");
}

static void path_update_tiles(graph_t* g, int tunnel) {
    int i;
    for(i = 0; i < g->size; i++) {
        vertex_t* v = g->vertices[i];
        if(v == NULL) {
            continue;
        }
        point_t p = index_to_point(v->index);
        tile_t* t = _dungeon_array[p.y][p.x];
        if(tunnel) {
            tileAPI.update_dist_tunnel(t, v->dist);
        } else {
            tileAPI.update_dist(t, v->dist);
        }
    }
}

pathfinder_namespace const pathfinderAPI = {
    path_construct,
    path_destruct,
    path_gen_map
};
