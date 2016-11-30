//
//  item_store.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/9/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "item_store.h"
#include "item.h"
#include "../parser/item_description.h"
#include "../parser/parser.h"
#include "../env/env.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"
#include "../logger/logger.h"

int item_store::ITEM_COUNT = 0;
int item_store::_items_size = 0;
int item_store::_items_len = 0;
item** item_store::_items = NULL;

void item_store::setup() {
    int numitems = 20;
    int i;
    _items_size = numitems;
    _items = (item**) calloc(_items_size, sizeof(*_items));
    _items_len = _items_size;
    ITEM_COUNT = _items_len;
    
    for(i = 0; i < _items_size; i++) {
        item* item = gen_item();
        setup_item(item);
        _items[i] = item;
    }
}

void item_store::teardown() {
    int i;
    for(i = 0; i < _items_size; i++) {
        if(_items[i] != NULL) {
            delete _items[i];
        }
    }
    free(_items);
}

void item_store::move_floors() {
    int i;
    for(i = 0; i < _items_size; i++) {
        if(_items[i] != NULL) {
            delete _items[i];
        }
    }
    free(_items);
    
    item_store::setup();
}

item** item_store::get_items() {
    return _items;
}

void item_store::pickup_item(point* p) {
    int i;
    int idx = -1;
    for(i = 0; i < _items_len; i++) {
        item* item = _items[i];
        if(item == NULL) {
            continue;
        }
        if(p->distance_to(item->position) == 0 && item->state == is_dropped) {
            int res = character::pc_pickup_item(item);
            if(res == 0) {
                idx = i;
                break;
            } else if(res == 2) {
                // TODO: Inventory is full, deal with error
            }
        }
    }
    if(idx > 0) {
        // item should be removed at index idx, shift over it and remove the last item in the list (will always be duplicate after shifting)
        for(i = idx; i < _items_len-1; i++) {
            _items[i] = _items[i+1];
        }
        _items[--_items_len] = NULL;
        logger::i("Picked up item at index %d", idx);
    }
}

void item_store::drop_item(item* i) {
    int c;
    if(i == NULL) {
        logger::w("item drop called with NULL item");
        return;
    }
    
    for(c = 0; c < _items_len; c++) {
        if(_items[c] == i) {
            logger::w("item drop called with item already dropped!");
            return;
        }
    }
    
    if(_items_len == _items_size) {
        int new_size = _items_size*2;
        item** new_items = (item**) realloc(_items, sizeof(*_items)*new_size);
        if(new_items == NULL) {
            // deal with error!
            return;
        }
        for(c = _items_size; c < new_size; c++) {
            new_items[c] = NULL;
        }
        _items = new_items;
        _items_size = new_size;
    }
    
    character* pc = character::get_pc();
    // update position to pc's position
    i->position->x = pc->position->x;
    i->position->y = pc->position->y;
    
    i->state = is_dropped;
    _items[_items_len++] = i;
}

bool item_store::contains_item(point* p) {
    int i;
    for(i = 0; i < _items_len; i++) {
        if(p->distance_to(_items[i]->position) == 0) {
            return true;
        }
    }
    return false;
}

item* item_store::gen_item() {
    item_description** item_list = parser::get_parser()->item_list;
    if(!env_constants::USE_IPARSE || item_list == NULL) {
        return new item(NULL);
    }
    
    int rand_idx = rand() % parser::get_parser()->item_len;
    item_description* descriptor = item_list[rand_idx];
    
    return new item(descriptor);
}

void item_store::setup_item(item* item) {
    point p(0, 0);
    dungeon::rand_point(dungeon::get_dungeon(), &p);
    
    if(item->position == NULL) {
        item->position = new point(p.x, p.y);
    }
    item->position->x = p.x;
    item->position->y = p.y;
}
