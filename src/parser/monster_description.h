//
//  monster_description.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef monster_description_h
#define monster_description_h

#include <string>

#include "dice.h"

class monster_description {
private:
    dice* speed;
    dice* hitpoints;
    dice* damage;
    char symb;
    std::string desc;
    std::string name;
    int color;
    int attributes;
    
    std::string print_attributes();
    std::string print_color();
public:
    
    monster_description();
    ~monster_description();
    
    std::string raw_desc;
    int check;
    
    void print();
    
    void parse_speed(std::string str);
    void parse_hp(std::string str);
    void parse_damage(std::string str);
    void parse_attrs(std::string str);
    void parse_name(std::string str);
    void parse_symb(std::string str);
    void parse_desc(std::string str);
    void parse_color(std::string str);
};


#endif /* monster_description_h */
