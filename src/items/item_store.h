//
//  item_store.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/9/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef item_store_h
#define item_store_h

#include "item.h"
#include "../point/point.h"

class item_store {
private:
    static item** _items;
    static int _items_size;
    static int _items_len;
    
    static item* gen_item();
    
    static void setup_item(item* item);
    
public:
    static int ITEM_COUNT;
    
    static void setup();
    static void teardown();
    static bool contains_item(point* p);
    static item** get_items();
    static void pickup_item(point* p);
    static void drop_item(item* i);
    static void move_floors();
};


#endif /* item_store_h */
