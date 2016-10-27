//
//  dijkstra.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "dijkstra.h"
#include "../graph/graph.h"
#include "../heap/heap.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"

static graph_t* construct(int invert) {
    graph_t* g = (graph_t*)calloc(1, sizeof(graph_t));
    
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
    free(g->vertices);
    free(g);
}

static path_node_t* bresenham(point_t* a, point_t* b) {
    point_t end = {a->x, a->y};
    point_t start = {b->x, b->y};
    int dx = abs(end.x - start.x);
    int sx = start.x < end.x ? 1 : -1;
    int dy = abs(end.y - start.y);
    int sy = start.y < end.y ? 1 : -1;
    int err = (dx < dy ? dx : dy)/2;
    int e2;
    path_node_t* head = graphAPI.add_path_node(&start);
    path_node_t* temp = head;
    while(1) {
        if(start.x < 1 || start.x > DUNGEON_WIDTH-2 || start.y < 1 || start.y > DUNGEON_WIDTH-2) {
            // invalid state!
            graphAPI.destruct_path(head);
            return NULL;
        }
        if(start.x == end.x && start.y == end.y) break;
        e2 = err;
        if(e2 > -dx) {
            err -= dy;
            start.x += sx;
        }
        if(e2 < dy) {
            err += dx;
            start.y += sy;
        }
        temp->next = graphAPI.add_path_node(&start);
        temp = temp->next;
    }
    
    return head;
}

static void dijkstra(graph_t* g, point_t* a, point_t* b) {
    int i, j;
    int ia = g->point_to_index(a);
    // if no point b is passed in, we will visit all points
    int ib = b != NULL ? g->point_to_index(b) : -1;
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
        vertex_t* v = (vertex_t*)heapAPI.remove(h);
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
    heapAPI.destruct(h);
}

dijkstra_namespace const dijkstraAPI = {
    construct,
    destruct,
    dijkstra,
    bresenham
};
