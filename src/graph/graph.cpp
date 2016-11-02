//
//  graph.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <cstddef>

#include "graph.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../util/util.h"

edge::edge(int dest, int weight) {
    this->dest = dest;
    this->weight = weight;
}

vertex::vertex(int index) {
    edges = nullptr;
    edges_len = 0;
    edges_size = 0;
    dist = 0;
    prev = 0;
    visited = false;
    this->index = index;
}

vertex::~vertex() {
    int i;
    for(i = 0; i < edges_len; i++) {
        delete edges[i];
    }
    free(edges);
}

graph::~graph() {
    int i;
    for(i = 0; i < size; i++) {
        if(vertices[i] != nullptr) {
            delete vertices[i];
        }
    }
    free(vertices);
}

void graph::add_vertex(point* p) {
    if(p == nullptr) {
        logger::w("point was nullptr! not adding vertex");
    }
    
    int i = util::point_to_index(p);
    int j;
    if (size < i + 1) {
        int new_size = size * 2 > i ? size * 2 : i + 4;
        vertices = (vertex**)realloc(vertices, new_size * sizeof (*vertices));
        for (j = size; j < new_size; j++)
            vertices[j] = nullptr;
        size = new_size;
    }
    if (vertices[i] == nullptr) {
        vertices[i] = new vertex(i);
        len++;
        logger::t("Added vertex at index: %d", i);
    }
}

void graph::add_edge(point* src, point* dest, int weight) {
    if(src == nullptr || dest == nullptr) {
        logger::w("src or dest point was nullptr! could not add edge");
        return;
    }
    
    add_vertex(src);
    add_vertex(dest);
    vertex* v = vertices[util::point_to_index(src)];
    if(v->edges_len >= v->edges_size) {
        v->edges_size = v->edges_size ? v->edges_size * 2 : 5;
        // TODO: Deal with this possible realloc error
        v->edges = (edge**)realloc(v->edges, v->edges_size * sizeof (*v->edges));
    }
    edge* e = new edge(util::point_to_index(dest), weight);
    v->edges[v->edges_len++] = e;
    edge_count++;
}

int graph::point_to_index(point* p) {
    return util::point_to_index(p);
}

path_node::path_node(point* p) {
    if(p == nullptr) {
        logger::w("Tried to create path node with nullptr point!");
        return;
    }
    curr = new point(p->x, p->y);
    next = nullptr;
}

path_node::~path_node() {
    if(curr != nullptr) {
        free(curr);
    }
    if(next != nullptr) {
        delete next;
    }
}
