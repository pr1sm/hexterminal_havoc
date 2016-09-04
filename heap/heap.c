//
//  heap.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "heap.h"

#define HEAP_START_SIZE 128

static void percolate_up(heap_t* h, int index) {
    int parent;
    heap_node_t* tmp;
    for(parent = (index - 1) / 2;
        index && h->compare(h->array[index]->data, h->array[parent]->data) < 0;
        index = parent, parent = (index - 1) / 2) {
        tmp = h->array[index];
        h->array[index] = h->array[parent];
        h->array[parent] = tmp;
        h->array[index]->index = index;
        h->array[parent]->index = parent;
    }
}

static void percolate_down(heap_t* h, int index) {
    int child;
    void* tmp;
    
    for(child = (2 * index) + 1;
        child < h->size;
        index = child, child = (2 * index) + 1) {
        if(child + 1 < h->size &&
           h->compare(h->array[child]->data, h->array[child+1]->data) > 0) {
            child++;
        }
        if(h->compare(h->array[index]->data, h->array[child]->data) > 0) {
            tmp = h->array[index];
            h->array[index] = h->array[child];
            h->array[child] = tmp;
            h->array[index]->index = index;
            h->array[child]->index = child;
        }
    }
}

static void heapify(heap_t* h) {
    int i;
    for(i = (h->size + 1) / 2; i; i--) {
        percolate_down(h, i);
    }
    percolate_down(h, 0);
}

void heap_construct(heap_t* h,
                    int (*compare)(const void* a, const void* b),
                    void (*data_delete)(void *)) {
    h->size = 0;
    h->arr_size = HEAP_START_SIZE;
    h->compare = compare;
    h->data_delete=  data_delete;
    h->array = calloc(h->arr_size, sizeof(*h->array));
}

void heap_construct_from_array(heap_t* h,
                               void* array,
                               int size,
                               int num_members,
                               int (*compare)(const void* a, const void* b),
                               void (*data_delete)(void*)) {
    int i;
    char* a;
    
    h->size = h->arr_size = num_members;
    h->compare = compare;
    h->data_delete = data_delete;
    h->array = calloc(h->arr_size, sizeof(*h->array));
    
    for(i = 0, a = array; i < h->size; i++) {
        h->array[i] = malloc(sizeof(*h->array[i]));
        h->array[i]->index = i;
        h->array[i]->data = a + (i*size);
    }
    
    heapify(h);
}

void heap_destruct(heap_t* h) {
    int i;
    for(i = 0; i < h->size; i++) {
        if(h->data_delete) {
            h->data_delete(h->array[i]->data);
        }
        free(h->array[i]);
    }
    free(h->array);
    memset(h, 0, sizeof(*h));
}

heap_node_t* heap_insert(heap_t* h, void* v) {
    heap_node_t** tmp;
    heap_node_t* retval;
    
    if(h->size == h->arr_size) {
        h->arr_size *= 2;
        tmp = realloc(h->array, h->arr_size * sizeof(*h->array));
        if(!tmp) {
            // TODO log error
        } else {
            h->array = tmp;
        }
    }
    
    h->array[h->size] = retval = malloc(sizeof(*h->array[h->size]));
    h->array[h->size]->data = v;
    h->array[h->size]->index = h->size;
    
    percolate_up(h, h->size);
    h->size++;
    
    return retval;
}

void* heap_peek(heap_t* h) {
    return h->size ? h->array[0]->data : NULL;
}

void* heap_remove(heap_t* h) {
    void* tmp;
    if(!h->size) {
        return NULL;
    }
    
    tmp = h->array[0]->data;
    free(h->array[0]);
    h->size--;
    h->array[0] = h->array[h->size];
    percolate_down(h, 0);
    return tmp;
}

void heap_decrease_key(heap_t* h, heap_node_t* n) {
    percolate_up(h, n->index);
}

int heap_is_empty(heap_t* h) {
    return !h->size;
}

heap_namespace const heapAPI = { heap_construct, heap_construct_from_array, heap_destruct, heap_insert, heap_peek, heap_remove, heap_decrease_key, heap_is_empty };
