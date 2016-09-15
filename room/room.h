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
    uint8_t width;
    uint8_t height;
    uint8_t connected;
} room_t;

typedef struct room_namespace {
    room_t* (*const construct)(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    void (*const destruct)(room_t* room);
    int  (*const is_overlap)(room_t* r1, room_t* r2);
    int  (*const contains)(room_t* r, point_t* p);
    void (*const export_room)(room_t* r, uint8_t* data);
} room_namespace;
extern room_namespace const roomAPI;

#endif /* room_h */
