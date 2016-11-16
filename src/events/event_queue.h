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
#include "../heap/heap.h"

typedef int event_counter_t;

class EventQueueComparator : comparator<character> {
    virtual int compare(const character* c1, const character* c2) {
        return c1->event_count - c2->event_count;
    }
};

class event_queue {
private:
    static heap<character>* _event_queue;
    static EventQueueComparator* _eqc;
    
public:
    static event_counter_t EVENT_TIME;
    
    static void add_event(character* c);
    static int  perform_event();
    static void teardown();
    static void move_floors();
};

#endif /* event_queue_h */
