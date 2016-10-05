//
//  event_queue.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/4/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "event_queue.h"
#include "event.h"
#include "../character/character.h"
#include "../heap/heap.h"
#include "../logger/logger.h"

event_counter_t EVENT_TIME = 0;

static heap_t* _event_queue = NULL;

static int cmp_events(const void* e1, const void* e2) {
    event_t* e11 = (event_t*) e1;
    event_t* e22 = (event_t*) e2;
    return e11->event_count - e22->event_count;
}

static void remove_event(void* e) {
    eventAPI.destruct(e);
}

static void add_event_impl(character_t* c, event_action_t action) {
    if(_event_queue == NULL) {
        _event_queue = heapAPI.construct(cmp_events, remove_event);
    }
    event_t* e = eventAPI.contruct(c, action);
    heapAPI.insert(_event_queue, e);
}

static void perform_event_impl() {
    if(heapAPI.is_empty(_event_queue)) {
        logger.t("perform event called, but there are no events in the event queue!");
        return;
    }
    event_t* e = heapAPI.remove(_event_queue);
    
    e->perform(e);
    
    eventAPI.destruct(e);
}

event_queue_namespace const eventQueueAPI = {
    add_event_impl,
    perform_event_impl
};
