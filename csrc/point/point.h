//
//  point.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef point_h
#define point_h

#include "../env/env.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct point_t point_t;
struct point_t {
    // variables
    uint8_t x;
    uint8_t y;
    
    // functions
    int (*distance)(point_t* p1, point_t* p2);
    void (*print)(point_t* p);
};

typedef struct point_namespace {
    point_t* (*const construct)(uint8_t x, uint8_t y);
    int (*const destruct)(point_t* point);
} point_namespace;
extern point_namespace const pointAPI;
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* point_h */
