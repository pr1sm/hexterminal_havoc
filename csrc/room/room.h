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
#include "../env/env.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct room_t room_t;
struct room_t {
    // variables
    point_t* location;
    uint8_t width;
    uint8_t height;
    uint8_t connected;
    
    // functions
    int  (*is_overlap)(room_t* r1, room_t* r2);
    int  (*contains)(room_t* r, point_t* p);
    void (*export_room)(room_t* r, uint8_t* data);
};

typedef struct room_namespace {
    room_t* (*const construct)(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    void (*const destruct)(room_t* room);
} room_namespace;
extern room_namespace const roomAPI;
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* room_h */
