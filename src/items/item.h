//
//  item.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/9/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef item_h
#define item_h

#include <string>

#include "../parser/dice.h"
#include "../parser/item_description.h"
#include "../point/point.h"

class item {
public:
    object_type type;
    std::string name;
    std::string desc;
    char symb;
    int color;
    int hit;
    int dodge;
    int defense;
    int weight;
    int speed;
    int attribute;
    int value;
    dice* damage;
    bool is_equipment;
    bool picked_up;
    
    point* position;
    
    item(item_description* descriptor);
    ~item();
};


#endif /* item_h */
