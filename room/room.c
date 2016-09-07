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
#include "../logger/logger.h"

#define MIN_WIDTH 4
#define MIN_HEIGHT 3
#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80

room_t* room_construct(int x, int y, int width, int height) {
    room_t* r = (room_t*)malloc(sizeof(room_t));
    
    if(x < 1 || x >= DUNGEON_WIDTH - 1) {
        logger.w("Invalid x coord (%d) passed in, defaulting to 0", x);
        x = 0;
    }
    
    if(y < 1 || y >= DUNGEON_HEIGHT - 1) {
        logger.w("Invalid y coord (%d) passed in, defaulting to 0", y);
        y = 0;
    }
    
    if(width < MIN_WIDTH) {
        logger.w("Invalid width given (%d), should be at least %d.  Defaulting to minimum width", width, MIN_WIDTH);
        width = MIN_WIDTH;
    }
    
    if(height < MIN_HEIGHT) {
        logger.w("Invalid width given (%d), should be at least %d.  Defaulting to minimum width", height, MIN_HEIGHT);
        height = MIN_HEIGHT;
    }
    
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

int room_is_overlap(room_t* r1, room_t* r2) {
    int small_x, large_x, x_diff;
    int small_y, large_y, y_diff;
    int min_x_dim = (r1->width + r2->width + 1);
    int min_y_dim = (r1->height + r2->height + 1);
    
    small_x = r1->location->x < r2->location->x ? r1->location->x : r2->location->x;
    large_x = (r1->location->x + r1->width) > (r2->location->x + r2->width) ? (r1->location->x + r1->width) : (r2->location->x + r2->width);
    x_diff = large_x - small_x;
    
    small_y = r1->location->y < r2->location->y ? r1->location->y : r2->location->y;
    large_y = (r1->location->y + r1->height) > (r2->location->y + r2->height) ? (r1->location->y + r1->height) : (r2->location->y + r2->height);
    y_diff = large_y - small_y;
    
    logger.d("Comparing Rooms For Overlap");
    logger.t("Room1: x: %2d y: %2d w: %2d h: %2d", r1->location->x, r1->location->y, r1->width, r1->height);
    logger.t("Room2: x: %2d y: %2d w: %2d h: %2d", r2->location->x, r2->location->y, r2->width, r2->height);
    logger.t("s_x: %2d l_x: %2d d_x: %2d", small_x, large_x, x_diff);
    logger.t("s_y: %2d l_y: %2d d_y: %2d", small_y, large_y, y_diff);
    logger.t("min_x: %2d min_y: %2d", min_x_dim, min_y_dim);
    
    if(y_diff <= min_y_dim && x_diff <= min_x_dim) {
        logger.d("Overlap Detected!");
        return 1;
    }
    
    logger.d("No Overlap Detected!");
    return 0;
}

int room_contains(room_t* r, point_t* p) {
    logger.t("Checking room: (%2d, %2d, %2d, %2d) contains point: (%2d, %2d)...",
             r->location->x,
             r->location->y,
             r->width,
             r->height,
             p->x, p->y);
    int x_check = (p->x >= r->location->x) && (p->x < r->location->x + r->width);
    int y_check = (p->y >= r->location->y) && (p->y < r->location->y + r->height);
    if(x_check && y_check) {
        logger.t("Room contains point!");
        return 1;
    }
    logger.t("Room does not contain point!");
    return 0;
}

room_namespace const roomAPI = {
    room_construct,
    room_destruct,
    room_is_overlap
};
