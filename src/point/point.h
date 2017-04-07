//
//  point.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef point_h
#define point_h

#include "../env/env.h"

class point {
public:
    uint8_t x;
    uint8_t y;
    
    point(uint8_t x, uint8_t y);
    point(point* p);
    ~point();
    int  distance_to(point* p2);
    void print();
};

#endif /* point_h */
