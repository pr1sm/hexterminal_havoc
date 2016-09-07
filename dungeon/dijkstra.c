//
//  dijkstra.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>

#include "dijkstra.h"
#include "dungeon.h"
#include "../heap/heap.h"
#include "../tile/tile.h"
#include "../point/point.h"
#include "../logger/logger.h"

static int point_to_index(point_t* p) {
    // since outer rows and cols aren't being used
    // subtract one from both so the index starts at 0
    return ((p->y - 1) * DUNGEON_WIDTH) + (p->x - 1);
}

static int index_to_y(int index) {
    return (index / DUNGEON_WIDTH) + 1;
}

static int index_to_x(int index) {
    return (index % DUNGEON_WIDTH) + 1;
}

static void add_vertex(graph_t* g, point_t* p) {
    int i = point_to_index(p);
    if (g->size < i + 1) {
        int size = g->size * 2 > i ? g->size * 2 : i + 4;
        g->vertices = realloc(g->vertices, size * sizeof (vertex_t *));
        for (int j = g->size; j < size; j++)
            g->vertices[j] = NULL;
        g->size = size;
    }
    if (!g->vertices[i]) {
        g->vertices[i] = calloc(1, sizeof (vertex_t));
        g->vertices[i]->index = i;
        g->len++;
    }
}

static void add_edge(graph_t* g, point_t* p1, point_t* p2, int weight) {
    add_vertex(g, p1);
    add_vertex(g, p2);
    vertex_t* v = g->vertices[point_to_index(p1)];
    if(v->edges_len >= v->edges_size) {
        v->edges_size = v->edges_size ? v->edges_size * 2 : 5;
        // TODO: Deal with this possible realloc error
        v->edges = realloc(v->edges, v->edges_size * sizeof (*v->edges));
    }
    edge_t* e = calloc(1, sizeof(edge_t));
    e->dest = point_to_index(p2);
    e->weight = weight;
    v->edges[v->edges_len++] = e;
}

static int compare_vertices(const void *a, const void *b)
{
    const vertex_t* a_v = (const vertex_t*) a;
    const vertex_t* b_v = (const vertex_t*) b;
    return a_v->dist - b_v->dist;
}

static void teardown_vertex(vertex_t* v) {
    int i;
    for(i = 0; i < v->edges_len; i++) {
        free(v->edges[i]);
    }
    free(v);
}

graph_t* dijkstra_construct() {
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
                
                add_edge(g, t->location, dest->location, dest->rock_hardness);
            }
        }
    }
    
    return g;
}

void dijkstra_destruct(graph_t* g) {
    int i;
    for(i = 0; i < g->size; i++) {
        if(g->vertices[i] != NULL) {
            teardown_vertex(g->vertices[i]);
        }
    }
    free(g);
}

void dijkstra(graph_t* g, point_t* a, point_t* b) {
    int i, j;
    int ia = point_to_index(a);
    int ib = point_to_index(b);
    for(i = 0; i < g->size; i++) {
        // Catch the outer rows and cols
        if(g->vertices[i] == NULL) continue;
        vertex_t* v = g->vertices[i];
        v->dist = INT_MAX;
        v->prev = 0;
        v->visited = 0;
    }
    vertex_t* start = g->vertices[ia];
    vertex_t* end = NULL;
    start->dist = 0;
    heap_t* h = heapAPI.construct(compare_vertices, NULL);
    heapAPI.insert(h, start);
    while(h->size) {
        vertex_t* v = heapAPI.remove(h);
        
        if(v->index == ib) {
            end = v;
            break;
        }
        v->visited = 1;
        for(j = 0; j < v->edges_len; j++) {
            edge_t* e = v->edges[j];
            vertex_t* u = g->vertices[e->dest];
            if(!u->visited && ((v->dist + e->weight) <= u->dist)) {
                u->prev = v->index;
                u->dist = v->dist + e->weight;
                heapAPI.insert(h, u);
            }
        }
    }
}

void dijkstra_place_path(graph_t* g, point_t* b) {
    int n;
    vertex_t* v;
    v = g->vertices[point_to_index(b)];
    if(v->dist == INT_MAX) {
        logger.e("no path! exiting program");
        exit(1);
    }
    for(n = 1; v->dist; n++) {
        int x = index_to_x(v->index);
        int y = index_to_y(v->index);
        
        tile_t* tile = _dungeon_array[y][x];
        if(tile->content == tc_ROCK) {
            tileAPI.update_content(tile, tc_PATH);
        }
        v = g->vertices[v->prev];
        if(v == NULL) break;
    }
}

dijkstra_namespace const dijkstraAPI = {
    dijkstra_construct,
    dijkstra_destruct,
    dijkstra,
    dijkstra_place_path
} ;
