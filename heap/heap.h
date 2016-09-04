//
//  heap.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/3/16.
//  This is an implementation of the binary heap provided
//  to us through assignmet 1.03.  I've modified the variable
//  names so they make sense to me and adapted the functions
//  into the struct API notation I've been using.
//

#ifndef heap_h
#define heap_h

#include "node.h"

typedef struct heap_node_t {
    void* data;
    int index;
} heap_node_t;

typedef struct heap_t {
    heap_node_t** array;
    int size;
    int arr_size;
    int (*compare)(const void* a, const void* b);
    void (*data_delete)(void*);
} heap_t;

typedef struct heap_namespace {
    void  (*const construct)(heap_t* h,
                             int (*compare)(const void* a, const void* b),
                             void (*data_delete)(void*));
    void  (*const construct_from_array)(heap_t* h,
                                        void* array,
                                        int size,
                                        int num_members,
                                        int (*compare)(const void* a, const void* b),
                                        void (*data_delete)(void*));
    void  (*const destruct)(heap_t* h);
    heap_node_t* (*const insert)(heap_t* h, void* v);
    void* (*const peek)(heap_t* h);
    void* (*const remove)(heap_t* h);
    void  (*const decrease_key)(heap_t* h, heap_node_t* n);
    int   (*const is_empty)(heap_t*);
} heap_namespace;
extern heap_namespace const heapAPI;

#endif /* heap_h */
