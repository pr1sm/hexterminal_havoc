//
//  dice.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "dice.h"
#include "../logger/logger.h"

dice::dice(std::string str) {
    num_dice = 0;
    base = 0;
    sides = 0;
    char buf[70]; // large enough to capture all beginning space;
    sscanf(str.c_str(), "%s%d+%dd%d", buf, &base, &num_dice, &sides);
}

dice::dice(dice* dice) {
    num_dice = dice->num_dice;
    base     = dice->base;
    sides    = dice->sides;
}

int dice::roll() {
    int roll = base;
    int i;
    for(i = 0; i < num_dice; i++) {
        roll += (rand() % sides) + 1;
    }
    return roll;
}

std::string dice::to_string() {
    std::string str;
    char buf[20];
    sprintf(buf, "%d+%dd%d", base, num_dice, sides);
    str.append(buf);
    return str;
}
