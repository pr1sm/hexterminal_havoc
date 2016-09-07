//
//  room.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef room_h
#define room_h

#include "../point/point.h"

typedef struct room_t {
    point_t* location;
    int width;
    int height;
} room_t;

typedef struct room_namespace {
    room_t* (*const construct)(int x, int y, int width, int height);
    void (*const destruct)(room_t* room);
    int  (*const is_overlap)(room_t* r1, room_t* r2);
    int  (*const contains)(room_t* r, point_t* p);
} room_namespace;
extern room_namespace const roomAPI;

#endif /* room_h */
