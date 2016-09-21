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
#include "../graph/graph.h"
#include "../heap/heap.h"
#include "../point/point.h"
#include "../logger/logger.h"

static graph_t* construct(int invert) {
    graph_t* g = calloc(1, sizeof(graph_t));
    
    // Implement a general case!
    
    return g;
}

static void destruct(graph_t* g) {
    int i;
    for(i = 0; i < g->size; i++) {
        if(g->vertices[i] != NULL) {
            graphAPI.free_vertex(g->vertices[i]);
        }
    }
    free(g);
}

static void dijkstra(graph_t* g, point_t* a, point_t* b, int (*point_to_index)(point_t* p)) {
    int i, j;
    int ia = point_to_index(a);
    // if no point b is passed in, we will visit all points
    int ib = b != NULL ? point_to_index(b) : -1;
    for(i = 0; i < g->size; i++) {
        // Catch the outer rows and cols
        if(g->vertices[i] == NULL) continue;
        vertex_t* v = g->vertices[i];
        v->dist = INT_MAX;
        v->prev = 0;
        v->visited = 0;
    }
    vertex_t* start = g->vertices[ia];
    start->dist = 0;
    heap_t* h = heapAPI.construct(graphAPI.compare_vertices, NULL);
    heapAPI.insert(h, start);
    while(h->size) {
        vertex_t* v = heapAPI.remove(h);
        if(v->index == ib) {
            logger.i("Found Path with dist: %d", v->dist);
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

dijkstra_namespace const dijkstraAPI = {
    construct,
    destruct,
    dijkstra
} ;
