//
//  event_queue.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/4/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "event_queue.h"
#include "../character/character.h"
#include "../character/character_store.h"
#include "../heap/heap.h"
#include "../logger/logger.h"

event_counter_t EVENT_TIME = 0;

static heap_t* _event_queue = NULL;

static int cmp_events(const void* e1, const void* e2) {
    character_t* e11 = (character_t*) e1;
    character_t* e22 = (character_t*) e2;
    return e11->event_count - e22->event_count;
}

static void add_event_impl(character_t* c) {
    if(_event_queue == NULL) {
        _event_queue = heapAPI.construct(cmp_events, NULL);
    }
    c->event_count = EVENT_TIME + c->turn_count;
    heapAPI.insert(_event_queue, c);
}

static int perform_event_impl() {
    if(heapAPI.is_empty(_event_queue)) {
        logger.t("perform event called, but there are no events in the event queue!");
        return 0;
    }
    character_t* c = heapAPI.remove(_event_queue);
    EVENT_TIME = c->event_count;
    
    c->perform(c);
    
    // check if anything else should be moved
    c = heapAPI.peek(_event_queue);
    while(c->event_count == EVENT_TIME) {
        c = heapAPI.remove(_event_queue);
        c->perform(c);
        c = heapAPI.peek(_event_queue);
    }
    characterStoreAPI.npc_cleanup();
    return 1;
}

event_queue_namespace const eventQueueAPI = {
    add_event_impl,
    perform_event_impl
};
