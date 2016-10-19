//
//  pc_control.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/19/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include "pc_control.h"
#include "character_store.h"
#include "../events/event_queue.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"

void setup_control_movement() {
    eventQueueAPI.add_event(characterAPI.get_pc());
}

void handle_control_move() {
//    character_t** characters = characterStoreAPI.get_characters();
    character_t* pc = characterAPI.get_pc();
    // IMPLEMENT
    eventQueueAPI.add_event(pc);
}
