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
    int (*point_to_index)(point_t* p);
} graph_t;

// Path structs
typedef struct path_node_t path_node_t;
struct path_node_t {
    point_t* curr;
    path_node_t* next;
};

typedef struct graph_namespace {
    void (*const add_vertex)(graph_t* g, point_t* p);
    void (*const add_edge)(graph_t* g, point_t* src, point_t* dest, int weight);
    void (*const free_vertex)(vertex_t* v);
    int  (*const compare_vertices)(const void* a, const void* b);
    path_node_t* (*const add_path_node)(point_t* p);
    void (*const destruct_path)(path_node_t* pn);
} graph_namespace;
extern graph_namespace const graphAPI;

#endif /* graph_h */
