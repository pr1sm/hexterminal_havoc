//
//  item_description.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/6/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <ncurses.h>

#include "item_description.h"
#include "../character/character.h"

item_description::item_description() {
    hit = NULL;
    damage = NULL;
    dodge = NULL;
    defense = NULL;
    weight = NULL;
    speed = NULL;
    attribute = NULL;
    value = NULL;
    symb = '*';
    
    check = 0;
    is_equipment = false;
    raw_desc = "";
}

item_description::~item_description() {
    if(speed != NULL) {
        delete speed;
    }
    if(hit != NULL) {
        delete hit;
    }
    if(damage != NULL) {
        delete damage;
    }
    if(dodge != NULL) {
        delete dodge;
    }
    if(defense != NULL) {
        delete defense;
    }
    if(weight != NULL) {
        delete weight;
    }
    if(attribute != NULL) {
        delete attribute;
    }
    if(value != NULL) {
        delete value;
    }
}

void item_description::parse_type(std::string str) {
    // assuming TYPE is first...
    std::string type = str.substr(str.find_first_of(" \t")+1);
    
    if(type.compare(0, 6, "WEAPON") == 0) {
        this->type = ot_WEAPON;
        this->symb = '|';
        this->is_equipment = true;
    } else if(type.compare(0, 7, "OFFHAND") == 0) {
        this->type = ot_OFFHAND;
        this->symb = ')';
        this->is_equipment = true;
    } else if(type.compare(0, 6, "RANGED") == 0) {
        this->type = ot_WEAPON;
        this->symb = '}';
        this->is_equipment = true;
    } else if(type.compare(0, 5, "ARMOR") == 0) {
        this->type = ot_ARMOR;
        this->symb = '[';
        this->is_equipment = true;
    } else if(type.compare(0, 6, "HELMET") == 0) {
        this->type = ot_HELMET;
        this->symb = ']';
        this->is_equipment = true;
    } else if(type.compare(0, 5, "CLOAK") == 0) {
        this->type = ot_CLOAK;
        this->symb = '(';
        this->is_equipment = true;
    } else if(type.compare(0, 6, "GLOVES") == 0) {
        this->type = ot_GLOVES;
        this->symb = '{';
        this->is_equipment = true;
    } else if(type.compare(0, 5, "BOOTS") == 0) {
        this->type = ot_BOOTS;
        this->symb = '\\';
        this->is_equipment = true;
    } else if(type.compare(0, 4, "RING") == 0) {
        this->type = ot_RING;
        this->symb = '=';
        this->is_equipment = true;
    } else if(type.compare(0, 6, "AMULET") == 0) {
        this->type = ot_AMULET;
        this->symb = '"';
        this->is_equipment = true;
    } else if(type.compare(0, 5, "LIGHT") == 0) {
        this->type = ot_LIGHT;
        this->symb = '_';
        this->is_equipment = false;
    } else if(type.compare(0, 6, "SCROLL") == 0) {
        this->type = ot_SCROLL;
        this->symb = '~';
        this->is_equipment = false;
    } else if(type.compare(0, 4, "BOOK") == 0) {
        this->type = ot_BOOK;
        this->symb = '?';
        this->is_equipment = false;
    } else if(type.compare(0, 5, "FLASK") == 0) {
        this->type = ot_FLASK;
        this->symb = '!';
        this->is_equipment = false;
    } else if(type.compare(0, 4, "GOLD") == 0) {
        this->type = ot_GOLD;
        this->symb = '$';
        this->is_equipment = false;
    } else if(type.compare(0, 10, "AMMUNITION") == 0) {
        this->type = ot_AMMUNITION;
        this->symb = '/';
        this->is_equipment = false;
    } else if(type.compare(0, 4, "FOOD") == 0) {
        this->type = ot_FOOD;
        this->symb = ',';
        this->is_equipment = false;
    } else if(type.compare(0, 4, "WAND") == 0) {
        this->type = ot_WAND;
        this->symb = '-';
        this->is_equipment = false;
    } else if(type.compare(0, 9, "CONTAINER") == 0) {
        this->type = ot_CONTAINER;
        this->symb = '%';
        this->is_equipment = false;
    } else {
        this->type = ot_UNKNOWN;
        this->symb = '*';
        this->is_equipment = false;
    }
}

void item_description::parse_name(std::string str) {
    name = str.substr(str.find_first_of(" \t")+1);
}

void item_description::parse_desc(std::string str) {
    // assuming DESC is first...
    desc = str.substr(str.find_first_of(" \t")+1);
}

void item_description::parse_color(std::string str) {
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

void item_description::parse_hit(std::string str) {
    if(hit != NULL) {
        delete hit;
    }
    hit = new dice(str);
}

void item_description::parse_damage(std::string str) {
    if(damage != NULL) {
        delete damage;
    }
    damage = new dice(str);
}

void item_description::parse_dodge(std::string str) {
    if(dodge != NULL) {
        delete dodge;
    }
    dodge = new dice(str);
}

void item_description::parse_defense(std::string str) {
    if(defense != NULL) {
        delete defense;
    }
    defense = new dice(str);
}

void item_description::parse_weight(std::string str) {
    if(weight != NULL) {
        delete weight;
    }
    weight = new dice(str);
}

void item_description::parse_speed(std::string str) {
    if(speed != NULL) {
        delete speed;
    }
    speed = new dice(str);
}

void item_description::parse_attr(std::string str) {
    if(attribute != NULL) {
        delete attribute;
    }
    attribute = new dice(str);
}

void item_description::parse_value(std::string str) {
    if(value != NULL) {
        delete value;
    }
    value = new dice(str);
}

void item_description::print() {
    std::cout << "Name ~> " << name << std::endl;
    std::cout << "Type ~> " << print_type(type) << " (" << symb << ")" << std::endl;
    std::cout << "Weight ~> " << weight->to_string() << std::endl;
    std::cout << "Color ~> " << print_color() << std::endl;
    std::cout << "Dodge ~> " << dodge->to_string() << std::endl;
    std::cout << "Val ~> " << value->to_string() << std::endl;
    std::cout << "Dam ~> " << damage->to_string() << std::endl;
    std::cout << "Def ~> " << defense->to_string() << std::endl;
    std::cout << "Hit ~> " << hit->to_string() << std::endl;
    std::cout << "Speed ~> " << speed->to_string() << std::endl;
    std::cout << "Desc ~> " << std::endl << desc << std::endl;
    std::cout << "Attr ~> " << attribute->to_string() << std::endl << std::endl;
}

std::string item_description::print_color() {
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

std::string item_description::print_type(object_type type) {
    if(type == ot_AMMUNITION) {
        return "AMMUNITION";
    } else if(type == ot_AMULET) {
        return "AMULET";
    } else if(type == ot_ARMOR) {
        return "ARMOR";
    } else if(type == ot_BOOK) {
        return "BOOK";
    } else if(type == ot_BOOTS) {
        return "BOOTS";
    } else if(type == ot_CLOAK) {
        return "CLOAK";
    } else if(type == ot_CONTAINER) {
        return "CONTAINER";
    } else if(type == ot_FLASK) {
        return "FLASK";
    } else if(type == ot_GLOVES) {
        return "GLOVES";
    } else if(type == ot_GOLD) {
        return "GOLD";
    } else if(type == ot_HELMET) {
        return "HELMET";
    } else if(type == ot_LIGHT) {
        return "LIGHT";
    } else if(type == ot_OFFHAND) {
        return "OFFHAND";
    } else if(type == ot_RANGED) {
        return "RANGED";
    } else if(type == ot_RING) {
        return "RING";
    } else if(type == ot_SCROLL) {
        return "SCROLL";
    } else if(type == ot_WAND) {
        return "WAND";
    } else if(type == ot_WEAPON) {
        return "WEAPON";
    }
    return "UNKNOWN";
}
