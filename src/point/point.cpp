//
//  point.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "point.h"
#include "../logger/logger.h"
#include "../env/env.h"

point::point(uint8_t x, uint8_t y) {
    this->x = x;
    this->y = y;
}

point::point(point* p) {
    this->x = p->x;
    this->y = p->y;
}

point::~point() {
    
}

int point::distance_to(point* p) {
    int absxdiff = ((x - p->x) > 0) ? (x - p->x) : (p->x - x);
    int absydiff = ((y - p->y) > 0) ? (y - p->y) : (p->y - y);
    return (absxdiff > absydiff) ? absxdiff : absydiff; // Calculate distance with the fact that diagonals are length 1 too
}

void point::print() {
    logger::t("point: (%d, %d)", x, y);
    if(env_constants::DEBUG_MODE && !env_constants::NCURSES_MODE) {
        printf("point: (%d, %d)\n", x, y);
    }
}
