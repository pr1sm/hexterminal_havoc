//
//  pathing.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>
#include <math.h>


#include "pathing.h"
#include "../graph/graph.h"
#include "../heap/heap.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../util/util.h"

path_node* pathing::bresenham(point* end, point* start) {
    point t_start(start);
    point t_end(end);
    int dx = abs(t_end.x - t_start.x);
    int sx = t_start.x < t_end.x ? 1 : -1;
    int dy = abs(t_end.y - t_start.y);
    int sy = t_start.y < t_end.y ? 1 : -1;
    int err = (dx < dy ? dx : dy)/2;
    int e2;
    path_node* head = new path_node(&t_start);
    path_node* temp = head;
    while(1) {
        if(t_start.x < 1 || t_start.x > DUNGEON_WIDTH-2 || t_start.y < 1 || t_start.y > DUNGEON_HEIGHT-2) {
            delete head;
            return NULL;
        }
        if(t_start.x == t_end.x && t_start.y == t_end.y) break;
        
        e2 = err;
        if(e2 > -dx) {
            err -= dy;
            t_start.x += sx;
        }
        if(e2 < dy) {
            err += dx;
            t_start.y += sy;
        }
        temp->next = new path_node(&t_start);
        temp = temp->next;
    }
    
    return head;
}

int pathing::dijkstra(graph* g, point* a, point* b) {
    int i, j;
    int ia = util::point_to_index(a);
    int ib = b != NULL ? util::point_to_index(b) : -1;
    vertex* end = g->vertices[ia];
    if(end == NULL) {
        logger::e("start of dijkstra's algorithm isn't in this map! returning...");
        return 1;
    }
    for(i = 0; i < g->size; i++) {
        if(g->vertices[i] == NULL) {
            continue;
        }
        vertex* v = g->vertices[i];
        v->dist = INT_MAX;
        v->prev = 0;
        v->visited = false;
    }
    end->dist = 0;
    VertexComparator* vc = new VertexComparator();
    heap<vertex>* h = new heap<vertex>(vc, false);
    h->insert(end);
    while(!h->is_empty()) {
        vertex* v = h->remove();
        if(v->index == ib) {
            logger::i("Found Path with dist: %d", v->dist);
            break;
        }
        v->visited = true;
        for(j = 0; j < v->edges_len; j++) {
            edge* e = v->edges[i];
            vertex* u = g->vertices[e->dest];
            if(!u->visited && ((v->dist + e->weight) < u->dist)) {
                u->prev = v->index;
                u->dist = v->dist + e->weight;
                h->insert(u);
            }
        }
    }
    delete h;
    return 0;
}
