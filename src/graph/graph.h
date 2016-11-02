//
//  graph.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef graph_h
#define graph_h

#include "../point/point.h"
#include "../heap/heap.h"

class edge {
public:
    int dest;
    int weight;
    
    edge(int dest, int weight);
};

class vertex {
public:
    edge** edges;
    int edges_size;
    int edges_len;
    int index;
    int dist;
    int prev;
    bool visited;
    
    vertex(int index);
    ~vertex();
};

class graph {
public:
    vertex** vertices;
    int size;
    int len;
    int edge_count;
    
    ~graph();
    
    int point_to_index(point* p);
    
    void add_vertex(point* p);
    void add_edge(point* src, point* dest, int weight);
    
    static void compare_vertices(const void* a, const void* b);
};

class path_node {
public:
    point* curr;
    path_node* next;
    
    path_node(point* p);
    ~path_node();
};

class VertexComparator : comparator<vertex> {
    virtual int compare(const vertex* v1, const vertex* v2) {
        return v1->dist - v2->dist;
    }
};

#endif /* graph_h */
