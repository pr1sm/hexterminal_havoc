//
//  corridor.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>

#include "corridor.h"
#include "dungeon.h"
#include "../dijkstra/dijkstra.h"
#include "../graph/graph.h"
#include "../tile/tile.h"
#include "../point/point.h"
#include "../logger/logger.h"

static void place_path(graph_t* g, point_t* b);

static int point_to_index(point_t* p) {
    // since outer rows and cols aren't being used
    // subtract one from both so the index starts at 0
    return ((p->y - 1) * (DUNGEON_WIDTH-2)) + (p->x - 1);
}

static point_t index_to_point(int index) {
    point_t p = {(index % (DUNGEON_WIDTH-2))+1, (index / (DUNGEON_WIDTH-2))+1};
    return p;
}

static graph_t* construct(int invert) {
    logger.d("Constructing Graph for pathfinding...");
    graph_t* g = calloc(1, sizeof(graph_t));
    
    // add all edges to graph
    // just use adjacent tiles for now
    int i, j, k, x, y;
    int coord_adj[] = {-1, 0, 1, 0, -1};
    
    for(i = 1; i < DUNGEON_HEIGHT - 1; i++) {
        for(j = 1; j < DUNGEON_WIDTH - 1; j++) {
            tile_t* t = _dungeon_array[i][j];
            for(k = 0; k < 4; k++) {
                // check if we are in range
                y = i + coord_adj[k];
                x = j + coord_adj[k + 1];
                if(x < 1 || x >= DUNGEON_WIDTH - 1 || y < 1 || y >= DUNGEON_HEIGHT - 1) {
                    continue;
                }
                tile_t* dest = _dungeon_array[y][x];
                
                int weight = invert ? ROCK_MAX-dest->rock_hardness : dest->rock_hardness;
                graphAPI.add_edge(g, t->location, dest->location, weight, point_to_index);
            }
        }
    }
    
    logger.d("Final Vertex Count: %d", g->len);
    logger.d("Final Edge Count: %d", g->edge_count);
    logger.d("Graph for pathfinding constructed");
    return g;
}

static void destruct(graph_t* g) {
    logger.d("Destructing graph for pathfinding...");
    int i;
    for(i = 0; i < g->size; i++) {
        if(g->vertices[i] != NULL) {
            graphAPI.free_vertex(g->vertices[i]);
        }
    }
    free(g);
    logger.d("Graph for pathfinding destructed");
}

static void pathfind(graph_t* g, point_t* start, point_t* end) {
    dijkstraAPI.dijkstra(g, start, end, point_to_index);
    place_path(g, end);
}

static void place_path(graph_t* g, point_t* b) {
    int n;
    vertex_t* v;
    point_t p;
    v = g->vertices[point_to_index(b)];
    if(v->dist == INT_MAX) {
        logger.e("no path! exiting program");
        exit(1);
    }
    for(n = 1; v->dist; n++) {
        p = index_to_point(v->index);
        
        tile_t* tile = _dungeon_array[p.y][p.x];
        if(tile->content == tc_ROCK) {
            tileAPI.update_content(tile, tc_PATH);
        } else if(tile->content == tc_ROOM) {
            dungeonAPI.check_room_intercept(&p);
        }
        v = g->vertices[v->prev];
        if(v == NULL) break;
    }
}


corridor_namespace const corridorAPI = {
    construct,
    destruct,
    pathfind
};
