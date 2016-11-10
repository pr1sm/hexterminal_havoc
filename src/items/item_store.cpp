//
//  item_store.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/9/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "item_store.h"
#include "../parser/item_description.h"
#include "../parser/parser.h"
#include "../env/env.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"

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

item** item_store::get_items() {
    return _items;
}

void item_store::pickup_item(point* p) {
    int i;
    for(i = 0; i < _items_len; i++) {
        item* item = _items[i];
        if(p->distance_to(item->position) == 0) {
            item->picked_up = true;
        }
    }
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
