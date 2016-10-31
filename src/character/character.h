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

typedef int character_id_t;

#ifdef __cplusplus
extern "C" {

struct character_t;
typedef struct character_t character_t;

typedef struct character_namespace {
    character_t* (*const construct_npc)(point_t* spawn);
    void         (*const destruct)(character_t* c);
    character_t* (*const get_pc)();
    void         (*const teardown_pc)();
    char         (*const char_for_npc_type)(character_t* c);
    void         (*const perform)(character_t* c);
    
    // Accessors and Mutators
    character_id_t (*const get_id)(character_t* c);
    void           (*const set_id)(character_t* c, character_id_t id);
    
    character_type (*const get_type)(character_t* c);
    
    point_t*       (*const get_pos)(character_t* c);
    void           (*const set_pos)(character_t* c, point_t* p);
    
    point_t*       (*const get_dest)(character_t* c);
    void           (*const set_dest)(character_t* c, point_t* p);
    
    int            (*const get_event_count)(character_t* c);
    void           (*const set_event_count)(character_t* c, int event_count);
    
    uint8_t        (*const get_attrs)(character_t* c);
    uint8_t        (*const get_speed)(character_t* c);
    uint8_t        (*const get_turn_count)(character_t* c);
    uint8_t        (*const get_is_dead)(character_t* c);
    void           (*const set_is_dead)(character_t* c, uint8_t is_dead);
    
} character_namespace;
extern character_namespace const characterAPI;
    
}

class character {
public:
    character_id_t _id;
    character_type _type;
    point_t* _position;
    point_t* _destination;
    int _event_count;
    uint8_t _attrs;
    uint8_t _speed;
    uint8_t _turn_count;
    uint8_t _is_dead;
    
    character(character_type type, point_t* spawn);
    ~character();
    void set_position(point_t* p);
    void set_destination(point_t* p);
    void perform();
};

#endif // __cplusplus

#endif /* character_h */
