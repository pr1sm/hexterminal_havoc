//
//  point.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef point_h
#define point_h

typedef struct point_t {
    uint8_t x;
    uint8_t y;
} point_t;

typedef struct point_namespace {
    point_t* (*const construct)(uint8_t x, uint8_t y);
    int (*const destruct)(point_t* point);
    int (*const distance)(point_t* p1, point_t* p2);
    void (*const print)(point_t* p);
} point_namespace;
extern point_namespace const pointAPI;

#endif /* point_h */
