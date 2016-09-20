//
//  graph.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef graph_h
#define graph_h

#include "../point/point.h"

typedef struct {
    int dest;
    int weight;
} edge_t;

typedef struct {
    edge_t** edges;
    int edges_size;
    int edges_len;
    int index;
    int dist;
    int prev;
    int visited;
} vertex_t;

typedef struct {
    vertex_t** vertices;
    int size;
    int len;
    int edge_count;
} graph_t;

typedef struct graph_namespace {
    void (*const add_vertex)(graph_t* g, point_t* p, int (*point_to_index)(point_t* p));
    void (*const add_edge)(graph_t* g, point_t* src, point_t* dest, int weight, int (*point_to_index)(point_t* p));
    void (*const free_vertex)(vertex_t* v);
    int  (*const compare_vertices)(const void* a, const void* b);
} graph_namespace;
extern graph_namespace const graphAPI;

#endif /* graph_h */
