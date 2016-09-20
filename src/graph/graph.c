//
//  graph.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "graph.h"
#include "../point/point.h"
#include "../logger/logger.h"

void graph_add_vertex(graph_t* g, point_t* p, int (*point_to_index)(point_t* p)) {
    if(point_to_index == NULL) {
        logger.w("point_to_index was null, could not convert point to an index!");
        return;
    }
    
    if(g == NULL) {
        logger.w("graph was NULL! not adding vertex");
        return;
    }
    
    if(p == NULL) {
        logger.w("point was NULL! not adding vertex");
    }
    
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

void graph_free_vertex(vertex_t* v) {
    if(v == NULL) {
        logger.w("Could not free NULL vertex!");
        return;
    }
    int i;
    for(i = 0; i < v->edges_len; i++) {
        free(v->edges[i]);
    }
    free(v);
}

void graph_add_edge(graph_t* g, point_t* src, point_t* dest, int weight, int (*point_to_index)(point_t* p)) {
    if(point_to_index == NULL) {
        logger.w("point_to_index was null! could not add edge");
        return;
    }
    
    if(g == NULL) {
        logger.w("graph was null! could not add edge");
        return;
    }
    
    if(src == NULL || dest == NULL) {
        logger.w("src or dest point was NULL! could not add edge");
        return;
    }
    
    graphAPI.add_vertex(g, src, point_to_index);
    graphAPI.add_vertex(g, dest, point_to_index);
    vertex_t* v = g->vertices[point_to_index(src)];
    if(v->edges_len >= v->edges_size) {
        v->edges_size = v->edges_size ? v->edges_size * 2 : 5;
        // TODO: Deal with this possible realloc error
        v->edges = realloc(v->edges, v->edges_size * sizeof (*v->edges));
    }
    edge_t* e = calloc(1, sizeof(edge_t));
    e->dest = point_to_index(dest);
    // Invert the rock hardness based on flag
    e->weight = weight;
    v->edges[v->edges_len++] = e;
    g->edge_count++;
}

int graph_compare_vertices(const void* a, const void* b) {
    const vertex_t* a_v = (const vertex_t*)a;
    const vertex_t* b_v = (const vertex_t*)b;
    return a_v->dist - b_v->dist;
}

graph_namespace const graphAPI = {
    graph_add_vertex,
    graph_add_edge,
    graph_free_vertex,
    graph_compare_vertices
};