//
//  equip_list.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/16/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef equip_list_h
#define equip_list_h

#include "../items/item.h"

class equip_list {
private:
    item** all_items;
    
    item* swap(item* i1, int index);
public:
    int equip_list_length;
    
    equip_list();
    ~equip_list();
    
    item* equip(item* i);
    item* unequip(object_type type);
    item** equipped_items();
};


#endif /* equip_list_h */
