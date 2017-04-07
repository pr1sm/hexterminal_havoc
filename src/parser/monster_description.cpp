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
#include <ncurses.h>

#include "monster_description.h"
#include "../character/character.h"


void monster_description::print() {
    std::cout << "Name ~> " << name << std::endl;
    std::cout << "Symbol ~> " << symb << std::endl;
    std::cout << "Desc ~> " << std::endl << desc << std::endl;
    std::cout << "Color ~> " << print_color() << std::endl;
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

std::string monster_description::print_color() {
    if(color == COLOR_BLACK) {
        return "BLACK";
    } else if(color == COLOR_RED) {
        return "RED";
    } else if(color == COLOR_GREEN) {
        return "GREEN";
    } else if(color == COLOR_YELLOW) {
        return "YELLOW";
    } else if(color == COLOR_BLUE) {
        return "BLUE";
    } else if(color == COLOR_MAGENTA) {
        return "MAGENTA";
    } else if(color == COLOR_CYAN) {
        return "CYAN";
    } else if(color == COLOR_WHITE) {
        return "WHITE";
    }
    return "N/A";
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
    // assuming COLOR is first...
    std::string color = str.substr(str.find_first_of(" \t")+1);
    // only care about first color
    if(color.compare(0, 5, "BLACK") == 0) {
        this->color = COLOR_BLACK;
    } else if(color.compare(0, 3, "RED") == 0) {
        this->color = COLOR_RED;
    } else if(color.compare(0, 5, "GREEN") == 0) {
        this->color = COLOR_GREEN;
    } else if(color.compare(0, 6, "YELLOW") == 0) {
        this->color = COLOR_YELLOW;
    } else if(color.compare(0, 4, "BLUE") == 0) {
        this->color = COLOR_BLUE;
    } else if(color.compare(0, 7, "MAGENTA") == 0) {
        this->color = COLOR_MAGENTA;
    } else if(color.compare(0, 4, "CYAN") == 0) {
        this->color = COLOR_CYAN;
    } else if(color.compare(0, 5, "WHITE") == 0) {
        this->color = COLOR_WHITE;
    }
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
    color = COLOR_WHITE;
    attributes = 0;
    symb = '0';
    desc = "";
    name = "";
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