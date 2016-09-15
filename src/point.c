//
//  point.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
// Temporary!
#include <stdio.h>

#include "point.h"
#include "logger.h"

point_t* point_construct(uint8_t x, uint8_t y) {
    point_t* point = (point_t*)malloc(sizeof(point_t));
    point->x = x;
    point->y = y;
    return point;
}

int point_destruct(point_t* point) {
    free(point);
    return 0;
}

int point_distance(point_t* p1, point_t* p2) {
    return (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y);
}

void point_print(point_t* p) {
    logger.t("point: (%d, %d)", p->x, p->y);
    // Temporary!
    printf("point: (%d, %d)\n", p->x, p->y);
}

point_namespace const pointAPI = { point_construct, point_destruct, point_distance, point_print };
