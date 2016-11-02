//
//  room.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef room_h
#define room_h

#include "../point/point.h"
#include "../env/env.h"

class room {
public:
    point* location;
    uint8_t width;
    uint8_t height;
    uint8_t connected;
    
    room(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    ~room();
    
    int overlap_with(room* r);
    int contains(point* p);
    void export_room(uint8_t* data);
};

#endif /* room_h */
