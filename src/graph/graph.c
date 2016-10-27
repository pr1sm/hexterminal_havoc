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

static void add_vertex(graph_t* g, point_t* p) {
    if(g->point_to_index == NULL) {
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
    
    int i = g->point_to_index(p);
    int j;
    if (g->size < i + 1) {
        int size = g->size * 2 > i ? g->size * 2 : i + 4;
        g->vertices = (vertex_t**)realloc(g->vertices, size * sizeof (vertex_t *));
        for (j = g->size; j < size; j++)
            g->vertices[j] = NULL;
        g->size = size;
    }
    if (!g->vertices[i]) {
        g->vertices[i] = (vertex_t*)calloc(1, sizeof (vertex_t));
        g->vertices[i]->index = i;
        g->len++;
        logger.t("Added vertex at index: %d", i);
    }
}

static void free_vertex(vertex_t* v) {
    if(v == NULL) {
        logger.w("Could not free NULL vertex!");
        return;
    }
    int i;
    for(i = 0; i < v->edges_len; i++) {
        free(v->edges[i]);
    }
    free(v->edges);
    free(v);
}

static void add_edge(graph_t* g, point_t* src, point_t* dest, int weight) {
    if(g->point_to_index == NULL) {
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
    
    graphAPI.add_vertex(g, src);
    graphAPI.add_vertex(g, dest);
    vertex_t* v = g->vertices[g->point_to_index(src)];
    if(v->edges_len >= v->edges_size) {
        v->edges_size = v->edges_size ? v->edges_size * 2 : 5;
        // TODO: Deal with this possible realloc error
        v->edges = (edge_t**)realloc(v->edges, v->edges_size * sizeof (*v->edges));
    }
    edge_t* e = (edge_t*)calloc(1, sizeof(edge_t));
    e->dest = g->point_to_index(dest);
    // Invert the rock hardness based on flag
    e->weight = weight;
    v->edges[v->edges_len++] = e;
    g->edge_count++;
}

static path_node_t* add_path_node(point_t* p) {
    if(p == NULL) {
        logger.w("Tried to create path node with NULL point! Returning NULL");
        return NULL;
    }
    path_node_t* pn = (path_node_t*)calloc(1, sizeof(path_node_t));
    point_t* curr = pointAPI.construct(p->x, p->y);
    pn->curr = curr;
    pn->next = NULL;
    return pn;
}

static void destruct_path(path_node_t* pn) {
    free(pn->curr);
    if(pn->next != NULL) {
        graphAPI.destruct_path(pn->next);
    }
    free(pn);
}

static int compare_vertices(const void* a, const void* b) {
    const vertex_t* a_v = (const vertex_t*)a;
    const vertex_t* b_v = (const vertex_t*)b;
    return a_v->dist - b_v->dist;
}

graph_namespace const graphAPI = {
    add_vertex,
    add_edge,
    free_vertex,
    compare_vertices,
    add_path_node,
    destruct_path
};
