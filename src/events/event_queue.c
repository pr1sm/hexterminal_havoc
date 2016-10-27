//
//  event_queue.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/4/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "event_queue.h"
#ifdef __cplusplus
    #include "../character/character.h"
#else
    #include "../character/character_t.h"
#endif // __cplusplus
#include "../character/character_store.h"
#include "../heap/heap.h"
#include "../logger/logger.h"

event_counter_t EVENT_TIME = 0;

static heap_t* _event_queue = NULL;
static void character_perform(character_t* c);

static int cmp_events(const void* e1, const void* e2) {
    character_t* e11 = (character_t*) e1;
    character_t* e22 = (character_t*) e2;
    int ec1;
    int ec2;
#ifdef __cplusplus
    ec1 = characterAPI.get_event_count(e11);
    ec2 = characterAPI.get_event_count(e22);
#else
    ec1 = e11->event_count;
    ec2 = e22->event_count;
#endif // __cplusplus
    return ec1 - ec2;
}

static void add_event_impl(character_t* c) {
    if(_event_queue == NULL) {
        _event_queue = heapAPI.construct(cmp_events, NULL);
    }
    uint8_t tc;
#ifdef __cplusplus
    tc = characterAPI.get_turn_count(c);
    characterAPI.set_event_count(c, EVENT_TIME + tc);
#else
    tc = c->turn_count;
    c->event_count = EVENT_TIME + tc;
#endif // __cplusplus
    heapAPI.insert(_event_queue, c);
}

static int perform_event_impl() {
    int character_ec;
    if(heapAPI.is_empty(_event_queue)) {
        logger.t("perform event called, but there are no events in the event queue!");
        return 0;
    }
    character_t* c = (character_t*)heapAPI.remove(_event_queue);
#ifdef __cplusplus
    EVENT_TIME = characterAPI.get_event_count(c);
#else
    EVENT_TIME = c->event_count;
#endif // __cplusplus
    
    character_perform(c);
    
    // check if anything else should be moved
    c = (character_t*)heapAPI.peek(_event_queue);
#ifdef __cplusplus
    character_ec = characterAPI.get_event_count(c);
#else
    character_ec = c->event_count;
#endif // __cplusplus
    while(character_ec == EVENT_TIME) {
        c = (character_t*)heapAPI.remove(_event_queue);
        character_perform(c);
        c = (character_t*)heapAPI.peek(_event_queue);
        if(c == NULL) {
            break;
        }
#ifdef __cplusplus
        character_ec = characterAPI.get_event_count(c);
#else
        character_ec = c->event_count;
#endif // __cplusplus
    }
    characterStoreAPI.npc_cleanup();
    if(QUIT_FLAG == 1) {
        return 0;
    }
    return 1;
}

static void teardown_impl() {
    heapAPI.destruct(_event_queue);
}

static void move_floors_impl() {
    heapAPI.destruct(_event_queue);
    _event_queue = NULL;
    EVENT_TIME = 0;
}

static void character_perform(character_t* c) {
#ifdef __cplusplus
    characterAPI.perform(c);
#else
    c->perform(c);
#endif // __cplusplus
}

event_queue_namespace const eventQueueAPI = {
    add_event_impl,
    perform_event_impl,
    teardown_impl,
    move_floors_impl
};
