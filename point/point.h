//
//  point.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef point_h
#define point_h

typedef struct point {
    int x;
    int y;
} point_t;

typedef struct {
    point_t* (*const construct)(int x, int y);
    int (*const destruct)(point_t* point);
} point_namespace;
extern point_namespace const point;

#endif /* point_h */
