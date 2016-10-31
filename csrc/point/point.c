//
//  point.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "point.h"
#include "../logger/logger.h"
#include "../env/env.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Public Functions
static int distance_impl(point_t* p1, point_t* p2);
static void print_impl(point_t* p);

static point_t* construct_impl(uint8_t x, uint8_t y) {
    point_t* point = (point_t*)malloc(sizeof(point_t));
    point->x = x;
    point->y = y;
    
    point->distance = distance_impl;
    point->print = print_impl;
    return point;
}

static int destruct_impl(point_t* point) {
    free(point);
    return 0;
}

static int distance_impl(point_t* p1, point_t* p2) {
    int absxdiff = ((p1->x - p2->x) > 0) ? (p1->x - p2->x) : (p2->x - p1->x);
    int absydiff = ((p1->y - p2->y) > 0) ? (p1->y - p2->y) : (p2->y - p1->y);
    return (absxdiff > absydiff) ? absxdiff : absydiff; // Calculate distance with the fact that diagonals are length 1 too
//    return (absxdiff*absxdiff) + (absydiff*absydiff);
}

static void print_impl(point_t* p) {
    logger.t("point: (%d, %d)", p->x, p->y);
    if(DEBUG_MODE && !NCURSES_MODE) {
        printf("point: (%d, %d)\n", p->x, p->y);
    }
}

point_namespace const pointAPI = {
    construct_impl,
    destruct_impl
};
    
#ifdef __cplusplus
}
#endif // __cplusplus
