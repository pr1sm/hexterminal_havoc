//
//  event_queue.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/4/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef event_queue_h
#define event_queue_h

#include "../character/character.h"

typedef int event_counter_t;

extern event_counter_t EVENT_TIME;

typedef struct event_queue_namespace {
    void (*const add_event)(character_t* c);
    int  (*const perform_event)();
    void (*const teardown)();
    void (*const move_floors)();
} event_queue_namespace;
extern event_queue_namespace const eventQueueAPI;

#endif /* event_queue_h */
