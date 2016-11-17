//
//  equip_list.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/16/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "equip_list.h"
#include "../items/item.h"
#include "../logger/logger.h"

equip_list::equip_list() {
    equip_list_length = 12; // Weapon, Offhand, Ranged, Armor, Helmet, Cloak, Gloves, Boots, Amulet, Light, Ring, Ring
    all_items = (item**) calloc(equip_list_length, sizeof(*all_items));
}

equip_list::~equip_list() {
    int i;
    for(i = 0; i < equip_list_length; i++) {
        if(all_items[i] != NULL) {
            delete all_items[i];
        }
    }
    free(all_items);
}

item* equip_list::equip(item* i) {
    if(i == NULL) {
        logger::w("equip called with NULL value!");
        return NULL;
    }
    int switchIndex = (i->type == ot_WEAPON)  ?  0 :
                      (i->type == ot_OFFHAND) ?  1 :
                      (i->type == ot_RANGED)  ?  2 :
                      (i->type == ot_ARMOR)   ?  3 :
                      (i->type == ot_HELMET)  ?  4 :
                      (i->type == ot_CLOAK)   ?  5 :
                      (i->type == ot_GLOVES)  ?  6 :
                      (i->type == ot_BOOTS)   ?  7 :
                      (i->type == ot_AMULET)  ?  8 :
                      (i->type == ot_LIGHT)   ?  9 :
                      (i->type == ot_RING)    ? 10 : -1;
    
    if(switchIndex >= 0) {
        return swap(i, switchIndex, 0);
    }
    
    logger::w("equip called with invalid object type: %x", i->type);
    return i;
}

item* equip_list::unequip(item* i) {
    if(i == NULL) {
        logger::w("unequip called with NULL value!");
        return NULL;
    }
    int idx = -1;
    int j;
    for(j = 0; j < equip_list_length; j++) {
        if(i == all_items[j]) {
            idx = j;
        }
    }
    
    if(idx == -1) {
        logger::w("unequip called with item not in equipment list");
        return i;
    }
    
    int switchIndex = (i->type == ot_WEAPON)  ?  0 :
                      (i->type == ot_OFFHAND) ?  1 :
                      (i->type == ot_RANGED)  ?  2 :
                      (i->type == ot_ARMOR)   ?  3 :
                      (i->type == ot_HELMET)  ?  4 :
                      (i->type == ot_CLOAK)   ?  5 :
                      (i->type == ot_GLOVES)  ?  6 :
                      (i->type == ot_BOOTS)   ?  7 :
                      (i->type == ot_AMULET)  ?  8 :
                      (i->type == ot_LIGHT)   ?  9 :
                      (i->type == ot_RING)    ? 11 : -1;
    
    if(switchIndex >= 0) {
        return swap(NULL, switchIndex, idx);
    }
    
    logger::w("unequip called with invalid object type: %x", i->type);
    return NULL;
}

item* equip_list::swap(item* i1, int index, int ring_idx) {
    if(index < 0 || index >= equip_list_length) {
        logger::w("equip swap called with index out of bounds: %d!", index);
        return NULL;
    }
    
    if(index < 10) { // not a ring
        item* ret = all_items[index];
        all_items[index] = i1;
        return ret;
    } else if(index == 10) {
        item* ring1 = all_items[10]; // hard coded for now, could probably change this in the future
        item* ring2 = all_items[11];
        
        if(ring1 == NULL) {        // ring1 is open, use it
            all_items[10] = i1;
            return ring1;
        } else if(ring2 == NULL) { // ring2 is open, use it
            all_items[11] = i1;
            return ring2;
        } else {                   // ring1 and ring2 are both used, move ring2 to ring1, i1 to ring2 and return ring1
            all_items[10] = ring2;
            all_items[11] = i1;
            return ring1;
        }
    } else {
        item* ring = all_items[ring_idx];
        all_items[ring_idx] = i1;
        return ring;
    }
}

item** equip_list::equipped_items() {
    return all_items;
}
