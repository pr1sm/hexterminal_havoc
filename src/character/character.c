//
//  character.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "character.h"
#include "ai.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"

static void set_position_impl(character_t* self, point_t* p);
static void set_destination_impl(character_t* self, point_t* p);
static void perform_impl(character_t* c);

static character_t* gPLAYER_CHARACTER = NULL;

static character_t* construct_impl(character_type type, point_t* p) {
    character_t* c = calloc(1, sizeof(character_t));
    point_t* pos;
    c->type = type;
    if(type == PC) {
        c->speed = 10;
        c->attrs = 0;
    } else if(type == NPC) {
        c->speed = (rand() & 0xf) + 5;
        c->attrs = rand() & 0xf;
    } else {
        logger.w("Invalid type passed into character constructor! returning NULL!");
        free(c);
        return NULL;
    }
    c->turn_count = 100 / c->speed;
    
    if(p == NULL) {
        logger.w("NULL point passed into character constructor! returning NULL!");
        free(c);
        return NULL;
    } else {
        pos = pointAPI.construct(p->x, p->y);
        c->position = pos;
    }
    c->destination = NULL;
    
    c->set_position = set_position_impl;
    c->set_destination = set_destination_impl;
    c->perform = perform_impl;
    return c;
}

static character_t* construct_npc_impl(point_t* p) {
    return construct_impl(NPC, p);
}

static void destruct_impl(character_t* c) {
    if(c->position != NULL) {
        free(c->position);
    }
    if(c->destination != NULL) {
        free(c->destination);
    }
    free(c);
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
        gPLAYER_CHARACTER = construct_impl(PC, &spawn);
    }
    return gPLAYER_CHARACTER;
}

static char char_for_npc_type_impl(character_t* self) {
    if(self->type == PC) {
        return '@';
    }
    if(self->attrs < 10) {
        return '0'+self->attrs;
    }
    return 'a'+self->attrs-10;
}

static void set_position_impl(character_t* self, point_t* p) {
    if(p == NULL) {
        logger.w("NULL point passed into set_point! returning without changing!");
        return;
    }
    if(self->position == NULL) {
        self->position = pointAPI.construct(p->x, p->y);
    } else {
        self->position->x = p->x;
        self->position->y = p->y;
    }
}

static void set_destination_impl(character_t* self, point_t* p) {
    if(p == NULL) {
        logger.w("NULL point passed into set_point! returning without changing!");
        return;
    }
    if(self->destination == NULL) {
        self->destination = pointAPI.construct(p->x, p->y);
    } else {
        self->destination->x = p->x;
        self->destination->y = p->y;
    }
}

static void perform_impl(character_t* c) {
    if(c->type == PC) {
        handle_pc_move();
    } else {
        handle_npc_move(c);
    }
}

const character_namespace characterAPI = {
    construct_npc_impl,
    destruct_impl,
    get_pc_impl,
    char_for_npc_type_impl
};
