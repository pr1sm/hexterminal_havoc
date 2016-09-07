//
//  dijkstra.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef dijkstra_h
#define dijkstra_h

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
} graph_t;

typedef struct dijkstra_namespace {
    graph_t* (*const construct)();
    void     (*const destruct)(graph_t* g);
    void     (*const dijkstra)(graph_t* g, point_t* start, point_t* end);
    void     (*const place_path)(graph_t* g, point_t* end);
} dijkstra_namespace;
extern dijkstra_namespace const dijkstraAPI;

#endif /* dijkstra_h */
