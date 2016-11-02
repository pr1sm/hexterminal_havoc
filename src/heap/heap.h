//
//  heap.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  This is an implementation of the binary heap provided
//  to us through assignment 1.03.  I've modified the variable
//  names so they make sense to me and adapted the functions
//  into a class definition with templates.
//

#ifndef heap_h
#define heap_h

#include "../env/env.h"

template<class T>
class comparator {
public:
    virtual int compare(const T* a, const T* b);
};

template<class T>
class heap_node {
public:
    T*    data;
    int   index;
    bool  delete_data;
    
    heap_node(T* d, int idx);
    ~heap_node();
};

template<class T>
class heap {
private:
    heap_node<T>** _array;
    int _size;
    int _arr_size;
    comparator<T>* _comparator;
    bool _delete_data;
    
    void percolate_up(int index);
    void percolate_down(int index);
    void heapify();
    
public:
    heap(::comparator<T>* c,
         bool delete_data);
    heap(T* array,
         int size,
         int num_members,
         ::comparator<T>* c,
         bool delete_data);
    ~heap();
    
    heap_node<T>* insert(T* data);
    T* peek();
    T* remove();
    void  decrease_key(heap_node<T>* node);
    bool  is_empty();
};

#endif /* heap_h */
