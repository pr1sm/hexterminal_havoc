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

void event_queue::add_event(character* c) {
    if(_event_queue == NULL) {
        _event_queue = new heap<character>((comparator<character>*)new EventQueueComparator(), NULL);
    }
    uint8_t tc;
    tc = c->_turn_count;
    c->_event_count = EVENT_TIME + tc;
    _event_queue->insert(c);
}

int event_queue::perform_event() {
    int character_ec;
    if(_event_queue->is_empty()) {
        logger::t("perform event called, but there are no events in the event queue!");
        return 0;
    }
    character* c = _event_queue->remove();
    EVENT_TIME = c->_event_count;
    
    c->perform();
    
    // check if anything else should be moved
    c = _event_queue->peek();
    character_ec = c->_event_count;
    while(character_ec == EVENT_TIME) {
        c = _event_queue->remove();
        c->perform();
        c = _event_queue->peek();
        if(c == NULL) {
            break;
        }
        character_ec = c->_event_count;
    }
    character_store::npc_cleanup();
    if(env_constants::QUIT_FLAG == 1) {
        return 0;
    }
    return 1;
}

void event_queue::teardown() {
    if(_event_queue != NULL) {
        delete _event_queue;
        _event_queue = NULL;
    }
}

void event_queue::move_floors() {
    delete _event_queue;
    _event_queue = NULL;
    EVENT_TIME = 0;
}
