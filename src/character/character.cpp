//
//  character.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/25/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "character.h"
#include "ai.h"
#include "pc_control.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"

character::character(character_type type, point_t* spawn) {
    _type = type;
    if(_type == PC) {
        _speed = 10;
        _attrs = 0;
    } else if(type == NPC) {
        _speed = (rand() & 0xf) + 5;
        _attrs = rand() & 0xf;
    } else {
        logger.w("Invalid type passed into character constructor!");
        _speed = 1;
        _attrs = 0;
    }
    _turn_count = 100 / _speed;
    
    if(spawn == NULL) {
        logger.w("NULL point passed into character constructor!");
        _position = pointAPI.construct(0, 0);
    } else {
        _position = pointAPI.construct(spawn->x, spawn->y);
    }
    _destination = NULL;
}

character::~character() {
    if(_position != NULL) {
        pointAPI.destruct(_position);
    }
    if(_destination != NULL) {
        pointAPI.destruct(_destination);
    }
}

void character::set_position(point_t* p) {
    if(p == NULL) {
        logger.w("NULL point passed into set_position! position will remain unchanged");
        return;
    }
    if(_position == NULL) {
        _position = pointAPI.construct(p->x, p->y);
    } else {
        _position->x = p->x;
        _position->y = p->y;
    }
}

void character::set_destination(point_t* p) {
    if(p == NULL) {
        logger.w("NULL point passed into set_point! destructing point!");
        pointAPI.destruct(_destination);
        _destination = NULL;
        return;
    }
    if(_destination == NULL) {
        _destination = pointAPI.construct(p->x, p->y);
    } else {
        _destination->x = p->x;
        _destination->y = p->y;
    }
}

void character::perform() {
    if(_type == PC) {
        if(PC_AI_MODE) {
            handle_pc_move();
        } else {
            handle_control_move();
        }
    } else if(_type == NPC) {
        // handle_npc_move(<#character_t *c#>);
    } else {
        logger.w("performed called on character with invalid type! doing nothing");
    }
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static character_t* gPLAYER_CHARACTER = NULL;

static character_t* construct(character_type type, point_t* p) {
    character* c = new character(type, p);
    return (character_t*) c;
}

static character_t* construct_npc_impl(point_t* p) {
    return construct(NPC, p);
}

static void destruct_impl(character_t* c) {
    if(c == NULL) {
        logger.w("destruct called with NULL character!");
        return;
    }
    character* c1 = (character*)c;
    delete c1;
}

static character_t* get_pc_impl() {
    if(gPLAYER_CHARACTER == NULL) {
        point_t spawn;
        if(X_START < 80 && Y_START < 21) {
            spawn.x = X_START;
            spawn.y = Y_START;
        } else {
            dungeonAPI.rand_point(dungeonAPI.get_dungeon(), &spawn);
        }
        gPLAYER_CHARACTER = construct(PC, &spawn);
        characterAPI.set_id(gPLAYER_CHARACTER, 0);
    }
    return gPLAYER_CHARACTER;
}

static char char_for_npc_type_impl(character_t* self) {
    if(characterAPI.get_type(self) == PC) {
        return '@';
    }
    if(characterAPI.get_attrs(self) < 10) {
        return '0'+ characterAPI.get_attrs(self);
    }
    return 'a'+characterAPI.get_attrs(self)-10;
}
    
static void perform_impl(character_t* self) {
    // IMPLEMENT
}
    
static character_id_t get_id_impl(character_t* self) {
    character* c = (character*)self;
    return c->_id;
}

static void set_id_impl(character_t* self, character_id_t id) {
    character* c = (character*)self;
    c->_id = id;
}

static character_type get_type_impl(character_t* self) {
    character* c = (character*)self;
    return c->_type;
}

static point_t* get_pos_impl(character_t* self) {
    character* c = (character*)self;
    return c->_position;
}

static void set_pos_impl(character_t* self, point_t* p) {
    character* c = (character*)self;
    c->set_position(p);
}

static point_t* get_dest_impl(character_t* self) {
    character* c = (character*)self;
    return c->_destination;
}

static void set_dest_impl(character_t* self, point_t* p) {
    character* c = (character*)self;
    c->set_destination(p);
}

static int get_event_count_impl(character_t* self) {
    character* c = (character*)self;
    return c->_event_count;
}

static void set_event_count_impl(character_t* self, int event_count) {
    character* c = (character*)self;
    c->_event_count = event_count;
}

static uint8_t get_attrs_impl(character_t* self) {
    character* c = (character*)self;
    return c->_attrs;
}

static uint8_t get_speed_impl(character_t* self) {
    character* c = (character*)self;
    return c->_speed;
}

static uint8_t get_turn_count_impl(character_t* self) {
    character* c = (character*)self;
    return c->_turn_count;
}

static uint8_t get_is_dead_impl(character_t* self) {
    character* c = (character*)self;
    return c->_is_dead;
}

static void set_is_dead_impl(character_t* self, uint8_t is_dead) {
    character* c = (character*)self;
    c->_is_dead = is_dead;
}

const character_namespace characterAPI = {
    construct_npc_impl,
    destruct_impl,
    get_pc_impl,
    char_for_npc_type_impl,
    perform_impl,
    
    // Accessors and Mutators
    get_id_impl,
    set_id_impl,
    get_type_impl,
    get_pos_impl,
    set_pos_impl,
    get_dest_impl,
    set_dest_impl,
    get_event_count_impl,
    set_event_count_impl,
    get_attrs_impl,
    get_speed_impl,
    get_turn_count_impl,
    get_is_dead_impl,
    set_is_dead_impl
};
    
#ifdef __cplusplus
}
#endif // __cplusplus
