//
//  room.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "room.h"
#include "../point/point.h"

room_t* room_construct(int x, int y, int width, int height) {
    room_t* r = (room_t*)malloc(sizeof(room_t));
    point_t* location = pointAPI.construct(x, y);
    r->location = location;
    r->width = width;
    r->height = height;
    return r;
}

void room_destruct(room_t* room) {
    free(room->location);
    free(room);
}

room_namespace const roomAPI = {
    room_construct,
    room_destruct
};
