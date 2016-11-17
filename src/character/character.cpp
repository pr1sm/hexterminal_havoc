//
//  character.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/25/16.
//  Copyright � 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <ncurses.h>

#include "character.h"
#include "ai.h"
#include "pc_control.h"
#include "equip_list.h"
#include "character_store.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../parser/monster_description.h"
#include "../parser/dice.h"
#include "../items/item.h"
#include "../items/item_store.h"

#define PC_CHAR '@'

static character* gPLAYER_CHARACTER = NULL;

character::character(character_type type, point* spawn) {
    this->type = type;
    if(type== PC) {
        speed = 10;
        base_speed = speed;
        attrs = 0;
        name = "player";
        desc = "the player character";
        color = COLOR_BLACK;
        hitpoints = 100;
        damage = new dice("DAMAGE 4+0d0"); // constant damage
        symb = PC_CHAR;
        is_seen = true;
    } else if(type == NPC) {
        speed = (rand() & 0xf) + 5;
        base_speed = speed;
        attrs = rand() & 0xf;
        name = "Random Monster";
        desc = "a monster with random abilities";
        color = COLOR_BLACK;
        hitpoints = 50;
        damage = new dice("DAMAGE 50+0d0"); // constant damage
        symb = char_for_npc_type();
        is_seen = false;
    } else {
        logger::w("Invalid type passed into character constructor!");
        speed = 1;
        base_speed = speed;
        attrs = 0;
        name = "Software bug";
        desc = "something is wrong";
        color = COLOR_RED;
        hitpoints = 10000;
        damage = new dice("DAMAGE 1+0d0"); // constant damage
        symb = 'z';
        is_seen = true;
    }
    inventory = NULL;
    inventory_size = 0;
    inventory_len = 0;
    equipment = new equip_list();
    turn_count = 100 / speed;
    is_dead = 0;
    event_count = 0;
    id = -1;
    
    if(spawn == NULL) {
        logger::w("NULL point passed into character constructor!");
        position = new point(0, 0);
    } else {
        position = new point(spawn);
    }
    destination = NULL;
}

character::character(character_type type, point* spawn, monster_description* descriptor) {
    this->type = type;
    if(type == PC) {
        speed = 10;
        base_speed = speed;
        attrs = 0;
        name = "player";
        desc = "the player character";
        color = COLOR_BLACK;
        hitpoints = 100;
        damage = new dice("DAMAGE 4+0d0"); // constant damage
        symb = PC_CHAR;
        is_seen = true;
    } else if(type == NPC) {
        // Use descriptor
        if(descriptor == NULL) {
            logger::w("character constructor with NULL descriptor! generating random monster");
            speed = (rand() & 0xf) + 5;
            base_speed = speed;
            attrs = rand() & 0xf;
            name = "Random Monster";
            desc = "a monster with random abilities";
            color = COLOR_BLACK;
            hitpoints = 50;
            damage = new dice("DAMAGE 50+0d0"); // constant damage
            symb = char_for_npc_type();
        } else {
            speed     = descriptor->speed->roll();
            base_speed = speed;
            attrs     = descriptor->attributes;
            name      = descriptor->name;
            desc      = descriptor->desc;
            color     = descriptor->color;
            hitpoints = descriptor->hitpoints->roll();
            damage    = new dice(descriptor->damage);
            symb      = descriptor->symb;
        }
        is_seen = false;
    } else {
        logger::w("Invalid type passed into character constructor!");
        speed = 1;
        base_speed = speed;
        attrs = 0;
        name = "Software bug";
        desc = "something is wrong";
        color = COLOR_RED;
        hitpoints = 10000;
        damage = new dice("DAMAGE 50+0d0"); // constant damage
        symb = 'z';
        is_seen = true;
    }
    inventory = NULL;
    inventory_size = 0;
    inventory_len = 0;
    equipment = new equip_list();
    turn_count = 100 / speed;
    is_dead = 0;
    event_count = 0;
    id = -1;
    
    if(spawn == NULL) {
        logger::w("NULL point passed into character constructor!");
        position = new point(0, 0);
    } else {
        position = new point(spawn);
    }
    destination = NULL;
}

character::~character() {
    static int char_count = 0;
    int i;
    logger::i("character destructor called - %d", ++char_count);
    if(position != NULL) {
        delete position;
    }
    if(destination != NULL) {
        delete destination;
    }
    if(damage != NULL) {
        delete damage;
    }
    if(equipment != NULL) {
        delete equipment;
    }
    if(inventory != NULL) {
        for(i = 0; i < inventory_size; i++) {
            if(inventory[i] != NULL) {
                delete inventory[i];
            }
        }
        free(inventory);
    }
}

// includes attacking
void character::set_position(point* p) {
    if(p == NULL) {
        logger::w("NULL point passed into set_position! position will remain unchanged");
        return;
    }
    character* pc = character::get_pc();
    if(type == NPC) {
        if(pc->position->distance_to(p) == 0) { // target position is the same as the pc, attack instead
            int dmg = damage->roll();
            logger::i("npc %d attacked with %d damage!", id, dmg);
            pc->hitpoints -= dmg;
            return;
        }
    } else if(type == PC) {
        int i;
        bool attacked = false;
        character_id_t* characters = character_store::get_alive_characters();
        for(i = 0; i < character_store::CHARACTER_COUNT; i++) {
            character* npc = character_store::npc_for_id(characters[i]);
            if(npc == NULL) {
                continue;
            }
            if(npc->position->distance_to(p) == 0) {
                int dmg = calc_damage();
                logger::i("pc attacked npc %d with %d damage!", id, dmg);
                npc->hitpoints -= dmg;
                npc->is_dead = (npc->hitpoints <= 0);
                attacked = true;
            }
        }
        if(attacked) {
            return;
        }
    }
    
    if(position == NULL) {
        position = new point(p);
    } else {
        position->x = p->x;
        position->y = p->y;
    }
}

void character::set_destination(point* p) {
    if(p == NULL) {
        logger::w("NULL point passed into set_point! destructing point!");
        delete destination;
        destination = NULL;
        return;
    }
    if(destination == NULL) {
        destination = new point(p);
    } else {
        destination->x = p->x;
        destination->y = p->y;
    }
}

void character::perform() {
    if(type == PC) {
        if(env_constants::PC_AI_MODE) {
            ai::handle_pc_move();
        } else {
            pc_control::handle_control_move();
        }
    } else if(type == NPC) {
        ai::handle_npc_move(this);
    } else {
        logger::w("performed called on character with invalid type! doing nothing");
    }
}

character* character::get_pc() {
    if(gPLAYER_CHARACTER == NULL) {
        point spawn(0, 0);
        if(env_constants::X_START < 80 && env_constants::Y_START < 21) {
            spawn.x = env_constants::X_START;
            spawn.y = env_constants::Y_START;
            
            // check if this point is valid
            dungeon* d = dungeon::get_dungeon();
            if(d->tiles[spawn.y][spawn.x]->content == tc_ROCK) {
                logger::e("Spawn point from CLI is invalid, setting new random point in the dungeon!");
                dungeon::rand_point(d, &spawn);
            }
        } else {
            dungeon::rand_point(dungeon::get_dungeon(), &spawn);
        }
        gPLAYER_CHARACTER = new character(PC, &spawn);
        gPLAYER_CHARACTER->id = 0;
    }
    return gPLAYER_CHARACTER;
}

void character::teardown_pc() {
    if(gPLAYER_CHARACTER != NULL) {
        delete gPLAYER_CHARACTER;
        gPLAYER_CHARACTER = NULL;
    }
}

int character::pc_pickup_item(item* i) {
    if(i == NULL) {
        logger::w("PC pickup called with NULL item");
        return 1;
    }
    
    character* pc = get_pc();
    if(pc->inventory == NULL) {
        pc->inventory_size = 10;
        pc->inventory_len = 0;
        pc->inventory = (item**)calloc(pc->inventory_size, sizeof(*pc->inventory));
    }
    
    if(pc->inventory_size == pc->inventory_len) {
        logger::w("PC has full inventory already");
        return 2;
    }
    
    i->state = is_picked_up;
    pc->inventory[pc->inventory_len++] = i;
    return 0;
}

int character::pc_drop_item(item* i) {
    if(i == NULL) {
        logger::w("PC drop called with NULL item");
        return 1;
    }
    
    int c;
    int idx = -1;
    character* pc = get_pc();
    bool in_inventory = false;
    for(c = 0; c < pc->inventory_len; c++) {
        if(pc->inventory[c] == i) {
            in_inventory = true;
            idx = c;
            break;
        }
    }
    
    if(in_inventory == false) {
        logger::w("PC drop called with item not in inventory");
        return 2;
    }
    
    if(i->state != is_picked_up) {
        logger::w("PC drop called with item that is equipped");
        return 3;
    }
    
    // remove item from the inventory (shift others over it
    for(c = idx; c < pc->inventory_len-1; c++) {
        pc->inventory[c] = pc->inventory[c+1];
    }
    // last item in list will always be duplicate after shifting
    pc->inventory[--pc->inventory_len] = NULL;
    item_store::drop_item(i);
    return 0;
}

int character::pc_equip_item(item* i) {
    if(i == NULL) {
        logger::w("PC equip called with NULL item");
        return 1;
    }
    
    int c;
    int idx = 0;
    character* pc = get_pc();
    bool in_inventory = false;
    for(c = 0; c < pc->inventory_len; c++) {
        if(pc->inventory[c] == i) {
            idx = c;
            in_inventory = true;
            break;
        }
    }
    
    if(in_inventory == false) {
        logger::w("PC equip called with item not in inventory");
        return 2;
    }
    
    if(i->state != is_picked_up) {
        logger::w("PC equip called with item that is already equipped");
        return 3;
    }
    
    i->state = is_equipped;
    item* swapped = pc->equipment->equip(i);
    if(swapped != NULL) {
        swapped->state = is_picked_up;
    }
    // remove item from the inventory (shift others over it
    for(c = idx; c < pc->inventory_len-1; c++) {
        pc->inventory[c] = pc->inventory[c+1];
    }
    // last item in list will always be duplicate after shifting, put swapped item in that place
    pc->inventory[pc->inventory_len-1] = swapped;
    if(swapped == NULL) {
        pc->inventory_len--;
    }
    pc->update_stats();
    return 0;
}

int character::pc_unequip_item(item* i) {
    if(i == NULL) {
        logger::w("PC unequip called with NULL item");
        return 1;
    }
    
    character* pc = get_pc();
    
    if(i->state != is_equipped) {
        logger::w("PC unequip called with item not equipped");
        return 2;
    }
    
    item* swapped = pc->equipment->unequip(i);
    
    if(swapped == NULL) {
        // deal with error
        logger::w("PC unequip call returned NULL value!");
        return 3;
    }
    
    // if we are at max carrying, drop the item
    if(pc->inventory_len == pc->inventory_size) {
        item_store::drop_item(swapped);
        return 0;
    }
    
    // add it to inventory
    swapped->state = is_picked_up;
    pc->inventory[pc->inventory_len++] = swapped;
    pc->update_stats();
    return 0;
}

int character::pc_expunge_item(item* i) {
    if(i == NULL) {
        logger::w("PC expunge called with NULL item");
        return 1;
    }
    int c;
    int idx = -1;
    character* pc = get_pc();
    bool in_inventory = false;
    for(c = 0; c < pc->inventory_len; c++) {
        if(pc->inventory[c] == i) {
            in_inventory = true;
            idx = c;
            break;
        }
    }
    
    if(in_inventory == false) {
        logger::w("PC expunge called with item not in inventory");
        return 2;
    }
    
    if(i->state != is_picked_up) {
        logger::w("PC expunge called with item that is equipped");
        return 3;
    }
    
    // remove item from the inventory (shift others over it
    for(c = idx; c < pc->inventory_len-1; c++) {
        pc->inventory[c] = pc->inventory[c+1];
    }
    // last item in list will always be duplicate after shifting
    pc->inventory[--pc->inventory_len] = NULL;
    delete i;
    return 0;
}

char character::char_for_npc_type() {
    if(type == PC) {
        return '@';
    }
    if(attrs < 10) {
        return '0' + attrs;
    }
    return 'a' + attrs - 10;
}

char character::get_print_symb(int mode) {
    dungeon* d = dungeon::get_dungeon();
    tile* t = d->tiles[position->y][position->x];
    if(is_seen || !env_constants::USE_FOW) {
        return symb;
    }
    return t->char_for_content(mode);
}

void character::update_stats() {
    int i;
    int new_speed = base_speed;
    item** equipped_items = equipment->equipped_items();
    if(equipped_items != NULL) {
        for(i = 0; i < equipment->equip_list_length; i++) {
            item* itm = equipped_items[i];
            if(itm != NULL) {
                new_speed += itm->speed;
            }
        }
    }
    
    speed = new_speed <= 0 ? 1 : new_speed;
    turn_count = 100 / speed;
}

int character::calc_damage() {
    int i;
    int dmg = damage->roll();
    item** equipped_items = equipment->equipped_items();
    if(equipped_items != NULL) {
        for(i = 0; i < equipment->equip_list_length; i++) {
            item* itm = equipped_items[i];
            if(itm != NULL) {
                dmg += itm->damage->roll();
            }
        }
    }
    
    return dmg;
}
