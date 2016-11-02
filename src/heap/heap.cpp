//
//  heap.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <cstddef>

#include "heap.h"

#define HEAP_START_SIZE 128

template<class T>
heap_node<T>::heap_node(T* d, int idx) {
    data = d;
    index = idx;
}

template<class T>
heap_node<T>::~heap_node() {
    
}

template<class T>
void heap<T>::percolate_up(int index) {
    int parent;
    heap_node<T>* tmp;
    for(parent = (index - 1) / 2;
        index && _comparator->compare(_array[index]->data, _array[parent]->data) < 0;
        index = parent, parent = (index - 1) / 2) {
        tmp = _array[index];
        _array[index] = _array[parent];
        _array[parent] = tmp;
        _array[index]->index = index;
        _array[parent]->index = parent;
    }
}

template<class T>
void heap<T>::percolate_down(int index) {
    int child;
    heap_node<T>* tmp;
    
    for(child = (2 * index) + 1;
        child < _size;
        index = child, child = (2 * index) + 1) {
        if(child + 1 < _size &&
           _comparator->compare(_array[child]->data, _array[child+1]->data) > 0) {
            child++;
        }
        if(_comparator->compare(_array[index]->data, _array[child]->data) > 0) {
            tmp = _array[index];
            _array[index] = _array[child];
            _array[child] = tmp;
            _array[index]->index = index;
            _array[child]->index = child;
        }
    }
}

template<class T>
void heap<T>::heapify() {
    int i;
    for(i = (_size + 1) / 2; i; i--) {
        percolate_down(i);
    }
    percolate_down(0);
}

template<class T>
heap<T>::heap(comparator<T>* c, bool delete_data) {
    _size = 0;
    _arr_size = HEAP_START_SIZE;
    _comparator = c;
    _delete_data = delete_data;
    _array = (heap_node<T>**)calloc(_arr_size, sizeof(*_array));
}

template<class T>
heap<T>::heap(T* array, int size, int num_members, comparator<T>*c, bool delete_data) {
    int i;
    _size = _arr_size = num_members;
    _comparator = c;
    _delete_data = delete_data;
    _array = (heap_node<T>**)calloc(_arr_size, *_array);
    
    for(i = 0; i < _size; i++) {
        _array[i] = new heap_node<T>(&array[i], i);
    }
    heapify();
}

template<class T>
heap<T>::~heap() {
    int i;
    for(i = 0; i < _size; i++) {
        if(_delete_data) {
            delete _array[i]->data;
        }
        delete _array[i];
    }
    delete _array;
}

template<class T>
heap_node<T>* heap<T>::insert(T* data) {
    heap_node<T>** tmp;
    heap_node<T>* retval;
    int i;
    for(i = 0; i < _size; i++) {
        if(_array[i]->data == data) {
            heapify();
            retval = _array[i];
            return retval;
        }
    }
    
    if(_size == _arr_size) {
        _arr_size *= 2;
        tmp = (heap_node<T>**)realloc(_array, _arr_size*sizeof(*_array));
        if(!tmp) {
            // TODO log error
        } else {
            _array = tmp;
        }
    }
    
    _array[_size] = retval = new heap_node<T>(data, _size);
    
    percolate_up(_size);
    _size++;
    
    return retval;
}

template<class T>
T* heap<T>::peek() {
    return _size ? _array[0]->data : nullptr;
}

template<class T>
T* heap<T>::remove() {
    T* tmp;
    if(!_size) {
        return nullptr;
    }
    
    tmp = _array[0]->data;
    delete _array[0];
    _size--;
    _array[0] = _array[_size];
    percolate_down(0);
    
    return tmp;
}

template<class T>
void heap<T>::decrease_key(heap_node<T>* node) {
    percolate_up(node->index);
}

template<class T>
bool heap<T>::is_empty() {
    return !_size;
}
