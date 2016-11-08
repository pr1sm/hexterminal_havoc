//
//  item_description.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/6/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef item_description_h
#define item_description_h

#include <string>

#include "dice.h"

typedef enum object_type {
    ot_UNKNOWN    =       0,
    ot_AMMUNITION =     0x1,
    ot_AMULET     =     0x2,
    ot_ARMOR      =     0x4,
    ot_BOOK       =     0x8,
    ot_BOOTS      =    0x10,
    ot_CLOAK      =    0x20,
    ot_CONTAINER  =    0x40,
    ot_FLASK      =    0x80,
    ot_FOOD       =   0x100,
    ot_GLOVES     =   0x200,
    ot_GOLD       =   0x400,
    ot_HELMET     =   0x800,
    ot_LIGHT      =  0x1000,
    ot_OFFHAND    =  0x2000,
    ot_RANGED     =  0x4000,
    ot_RING       =  0x8000,
    ot_SCROLL     = 0x10000,
    ot_WAND       = 0x20000,
    ot_WEAPON     = 0x40000
} object_type;

class item_description {
private:
    object_type type;
    std::string name;
    std::string desc;
    char symb;
    int color;
    dice* hit;
    dice* damage;
    dice* dodge;
    dice* defense;
    dice* weight;
    dice* speed;
    dice* attribute;
    dice* value;
    
    std::string print_color();
    std::string print_type();
    
public:
    
    item_description();
    ~item_description();
    
    int check;
    bool is_equipment;
    std::string raw_desc;
    
    void print();
    
    void parse_type(std::string str);
    void parse_name(std::string str);
    void parse_desc(std::string str);
    void parse_color(std::string str);
    void parse_hit(std::string str);
    void parse_damage(std::string str);
    void parse_dodge(std::string str);
    void parse_defense(std::string str);
    void parse_weight(std::string str);
    void parse_speed(std::string str);
    void parse_attr(std::string str);
    void parse_value(std::string str);
};


#endif /* item_description_h */
