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

class event_queue {
private:
    static heap<character>* _event_queue;
    
public:
    static event_counter_t EVENT_TIME;
    
    static void add_event(character* c);
    static int  perform_event();
    static void teardown();
    static void move_floors();
};

class EventQueueComparator : comparator<character> {
    virtual int compare(const character* c1, const character* c2) {
        return c1->_event_count - c2->_event_count;
    }
};

#endif /* event_queue_h */
