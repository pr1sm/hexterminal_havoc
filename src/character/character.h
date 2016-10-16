//
//  character.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef character_h
#define character_h

#include "../point/point.h"

#define INTEL_BIT 0
#define TELEP_BIT 1
#define TUNNL_BIT 2
#define ERATC_BIT 3
#define INTEL_VAL (1 << INTEL_BIT)
#define TELEP_VAL (1 << TELEP_BIT)
#define TUNNL_VAL (1 << TUNNL_BIT)
#define ERATC_VAL (1 << ERATC_BIT)

typedef enum character_type {
    NONE, PC, NPC
} character_type;

typedef struct character_t character_t;
struct character_t {
    character_type type;
    point_t* position;
    point_t* destination;
    uint8_t attrs;
    uint8_t speed;
    uint8_t turn_count;
    uint8_t is_dead;
    void (*set_position)(character_t* c, point_t* p);
    void (*set_destination)(character_t* c, point_t* p);
};

typedef struct character_namespace {
    character_t* (*const construct_npc)(point_t* spawn);
    void         (*const destruct)(character_t* c);
    character_t* (*const get_pc)();
    char         (*const char_for_npc_type)(character_t* c);
} character_namespace;
extern character_namespace const characterAPI;

#endif /* character_h */
