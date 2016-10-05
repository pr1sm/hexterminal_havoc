//
//  event.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/4/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef event_h
#define event_h

#include "../point/point.h"
#include "../character/character.h"

typedef enum event_action_t {
    MOVE, TUNNEL, ATTACK
} event_action_t;

typedef struct event_t event_t;
struct event_t {
    character_t* controller;
    event_action_t action;
    int event_count;
    void (*perform)(event_t* self);
};

typedef struct event_namespace {
    event_t* (*const contruct)(character_t* controller, event_action_t action);
    void     (*const destruct)(event_t* e);
} event_namespace;
extern event_namespace const eventAPI;

#endif /* event_h */
