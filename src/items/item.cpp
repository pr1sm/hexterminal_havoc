//
//  item.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/9/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <ncurses.h>

#include "item.h"
#include "../parser/item_description.h"
#include "../parser/dice.h"
#include "../logger/logger.h"

item::item(item_description* descriptor) {
    if(descriptor == NULL) {
        logger::w("Item constructor called with NULL descriptor!");
        
        type = ot_UNKNOWN;
        name = "";
        desc = "";
        symb = '+';
        color = COLOR_RED;
        hit = 0;
        dodge = 0;
        defense = 0;
        weight = 0;
        speed = 0;
        attribute = 0;
        value = 0;
        damage = NULL;
        position = NULL;
        is_equipment = false;
        state = is_unknown;
        return;
    }
    
    type = descriptor->type;
    name = descriptor->name;
    desc = descriptor->desc;
    symb = descriptor->symb;
    color = descriptor->color;
    damage = new dice(descriptor->damage);
    hit = descriptor->hit->roll();
    dodge = descriptor->dodge->roll();
    defense = descriptor->defense->roll();
    weight = descriptor->weight->roll();
    speed = descriptor->speed->roll();
    attribute = descriptor->attribute->roll();
    value = descriptor->value->roll();
    is_equipment = descriptor->is_equipment;
    position = NULL;
    state = is_dropped;
}

item::~item() {
    if(damage != NULL) {
        delete damage;
    }
    if(position != NULL) {
        delete position;
    }
}


