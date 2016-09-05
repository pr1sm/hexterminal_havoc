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
} room_namespace;
extern room_namespace const roomAPI;

#endif /* room_h */
