//
//  monster_description.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <sstream>

#include "monster_description.h"
#include "../character/character.h"


void monster_description::print() {
    std::cout << "Name ~> " << name << std::endl;
    std::cout << "Symbol ~> " << symb << std::endl;
    std::cout << "Desc ~> " << std::endl << desc << std::endl;
    std::cout << "Color ~> " << color << std::endl;
    std::cout << "Speed ~> " << speed->to_string() << std::endl;
    std::cout << "Abilities ~> " << print_attributes() << std::endl;
    std::cout << "Hitpoints ~> " << hitpoints->to_string() << std::endl;
    std::cout << "Damage ~> " << damage->to_string() << std::endl << std::endl;
}

std::string monster_description::print_attributes() {
    std::stringstream ss;
    if(attributes & INTEL_VAL) {
        ss << "SMART ";
    }
    if(attributes & TELEP_VAL) {
        ss << "TELE ";
    }
    if(attributes & TUNNL_VAL) {
        ss << "TUNNEL ";
    }
    if(attributes & ERATC_VAL) {
        ss << "ERRATIC ";
    }
    if(attributes & (1<<PASS_BIT)) {
        ss << "PASS ";
    }
    
    return ss.str();
}

void monster_description::parse_speed(std::string str) {
    if(speed != NULL) {
        delete speed;
    }
    speed = new dice(str);
}

void monster_description::parse_hp(std::string str) {
    if(hitpoints != NULL) {
        delete hitpoints;
    }
    hitpoints = new dice(str);
}

void monster_description::parse_damage(std::string str) {
    if(damage != NULL) {
        delete damage;
    }
    damage = new dice(str);
}

void monster_description::parse_symb(std::string str) {
    symb = str[5];
}

void monster_description::parse_desc(std::string str) {
    // assuming DESC is first...
    desc = str.substr(str.find_first_of(" \t")+1);
}

void monster_description::parse_name(std::string str) {
    // assuming NAME is first...
    name = str.substr(str.find_first_of(" \t")+1);
}

void monster_description::parse_color(std::string str) {
    // assuming NAME is first...
    color = str.substr(str.find_first_of(" \t")+1);
}

void monster_description::parse_attrs(std::string str) {
    std::stringstream ss(str);
    int attr = 0;
    while(!ss.eof()) {
        std::string str2 = "";
        ss >> str2;
        if(str2.compare("TELE") == 0) {
            attr |= TELEP_VAL;
        } else if(str2.compare("TUNNEL") == 0) {
            attr |= TUNNL_VAL;
        } else if(str2.compare("SMART") == 0) {
            attr |= INTEL_VAL;
        } else if(str2.compare("ERRATIC") == 0) {
            attr |= ERATC_VAL;
        } else if(str2.compare("PASS") == 0) {
            attr |= (1 << PASS_BIT);
        }
    }
    attributes = attr;
}

monster_description::monster_description() {
    speed = NULL;
    hitpoints = NULL;
    damage = NULL;
}

monster_description::~monster_description() {
    if(damage != NULL) {
        delete damage;
    }
    if(speed != NULL) {
        delete speed;
    }
    if(hitpoints != NULL) {
        delete hitpoints;
    }
}