//
//  character_t.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/25/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef character_t_h
#define character_t_h

#include "../point/point.h"
#include "character_common.h"

typedef struct character_t character_t;
struct character_t {
    character_id_t id;
    character_type type;
    point_t* position;
    point_t* destination;
    int event_count;
    uint8_t attrs;
    uint8_t speed;
    uint8_t turn_count;
    uint8_t is_dead;
    void (*set_position)(character_t* c, point_t* p);
    void (*set_destination)(character_t* c, point_t* p);
    void (*perform)(character_t* c);
};

typedef struct character_namespace {
    character_t* (*const construct_npc)(point_t* spawn);
    void         (*const destruct)(character_t* c);
    character_t* (*const get_pc)();
    char         (*const char_for_npc_type)(character_t* c);
} character_namespace;
extern character_namespace const characterAPI;


#endif /* character_t_h */
