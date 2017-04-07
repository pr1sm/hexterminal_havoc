//
//  room.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "room.h"
#include "../point/point.h"
#include "../logger/logger.h"

#define MIN_WIDTH 4
#define MIN_HEIGHT 3
#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80

room::room(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    if(x < 1 || x >= DUNGEON_WIDTH - 1) {
        logger::w("Invalid x coord (%d) passed in, defaulting to 0", x);
        x = 0;
    }
    
    if(y < 1 || y >= DUNGEON_HEIGHT - 1) {
        logger::w("Invalid y coord (%d) passed in, defaulting to 0", y);
        y = 0;
    }
    
    if(width < MIN_WIDTH && !env_constants::LOAD_DUNGEON) {
        logger::w("Invalid width given (%d), should be at least %d.  Defaulting to minimum width", width, MIN_WIDTH);
        width = MIN_WIDTH;
    }
    
    if(height < MIN_HEIGHT && !env_constants::LOAD_DUNGEON) {
        logger::w("Invalid height given (%d), should be at least %d.  Defaulting to minimum height", height, MIN_HEIGHT);
        height = MIN_HEIGHT;
    }
    
    this->location = new point(x, y);
    this->width = width;
    this->height = height;
    this->connected = 0;
}

room::~room() {
    delete location;
}

int room::overlap_with(room* r) {
    int small_x, large_x, x_diff;
    int small_y, large_y, y_diff;
    int min_x_dim = (width + r->width + 1);
    int min_y_dim = (height + r->height + 1);
    
    small_x = location->x < r->location->x ? location->x : r->location->x;
    large_x = (location->x + width) > (r->location->x + r->width) ? (location->x + width) : (r->location->x + r->width);
    x_diff = large_x - small_x;
    
    small_y = location->y < r->location->y ? location->y : r->location->y;
    large_y = (location->y + height) > (r->location->y + r->height) ? (location->y + height) : (r->location->y + r->height);
    y_diff = large_y - small_y;
    
    logger::d("Comparing Rooms For Overlap");
    logger::t("Room1: x: %2d y: %2d w: %2d h: %2d", location->x, location->y, width, height);
    logger::t("Room2: x: %2d y: %2d w: %2d h: %2d", r->location->x, r->location->y, r->width, r->height);
    logger::t("s_x: %2d l_x: %2d d_x: %2d", small_x, large_x, x_diff);
    logger::t("s_y: %2d l_y: %2d d_y: %2d", small_y, large_y, y_diff);
    logger::t("min_x: %2d min_y: %2d", min_x_dim, min_y_dim);
    
    if(y_diff <= min_y_dim && x_diff <= min_x_dim) {
        logger::d("Overlap Detected!");
        return 1;
    }
    
    logger::d("No Overlap Detected!");
    return 0;
}

int room::contains(point* p) {
    logger::t("Checking room: (%2d, %2d, %2d, %2d) contains point: (%2d, %2d)...",
             location->x,
             location->y,
             width,
             height,
             p->x, p->y);
    int x_check = (p->x >= location->x) && (p->x < location->x + width);
    int y_check = (p->y >= location->y) && (p->y < location->y + height);
    if(x_check && y_check) {
        logger::t("Room contains point!");
        return 1;
    }
    logger::t("Room does not contain point!");
    return 0;
}

void room::export_room(uint8_t* data) {
    // ASSUME: data is an array with length of 4
    data[0] = location->x;
    data[1] = width;
    data[2] = location->y;
    data[3] = height;
}
