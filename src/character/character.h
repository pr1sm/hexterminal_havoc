//
//  character.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
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

typedef int character_id_t;

class character {
public:
    character_id_t _id;
    character_type _type;
    point* _position;
    point* _destination;
    int _event_count;
    uint8_t _attrs;
    uint8_t _speed;
    uint8_t _turn_count;
    uint8_t _is_dead;
    
    character(character_type type, point* spawn);
    ~character();
    void set_position(point* p);
    void set_destination(point* p);
    void perform();
    char char_for_npc_type();
    
    static character* get_pc();
    static void       teardown_pc();
};

#endif /* character_h */
