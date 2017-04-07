//
//  event_queue.cpp
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

event_counter_t event_queue::EVENT_TIME = 0;
heap<character>* event_queue::_event_queue = NULL;
EventQueueComparator* event_queue::_eqc = NULL;

void event_queue::add_event(character* c) {
    if(_event_queue == NULL) {
        _eqc = new EventQueueComparator();
        _event_queue = new heap<character>((comparator<character>*)_eqc, false);
    }
    uint8_t tc;
    tc = c->turn_count;
    c->event_count = EVENT_TIME + tc;
    _event_queue->insert(c);
}

int event_queue::perform_event() {
    if(_event_queue->is_empty()) {
        logger::t("perform event called, but there are no events in the event queue!");
        return 0;
    }
    // perform move
    character* c;
    do {
        c = _event_queue->remove();
        EVENT_TIME = c->event_count;
        c->perform();
        c = _event_queue->peek();
        if(c == NULL) {
            break;
        }
    } while(c->event_count == EVENT_TIME);
    
//    while(c->event_count == EVENT_TIME) {
//        EVENT_TIME = c->event_count;
//        c->perform();
//        c = _event_queue->peek();
//        if(c == NULL) {
//            break;
//        }
//        c = _event_queue->remove();
//    }
    character_store::npc_cleanup();
    if(env_constants::QUIT_FLAG == 1) {
        return 0;
    }
    return 1;
}

void event_queue::teardown() {
    if(_event_queue != NULL) {
        delete _eqc;
        delete _event_queue;
        _event_queue = NULL;
        _eqc = NULL;
    }
}

void event_queue::move_floors() {
    delete _event_queue;
    delete _eqc;
    _event_queue = NULL;
    EVENT_TIME = 0;
}
