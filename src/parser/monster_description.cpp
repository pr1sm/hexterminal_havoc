//
//  monster_description.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <iostream>

#include "monster_description.h"


void monster_description::print() {
    std::cout << name << std::endl;
    std::cout << symb << std::endl;
    std::cout << desc << std::endl;
    std::cout << color << std::endl;
    std::cout << speed << std::endl;
    std::cout << abilities << std::endl;
    std::cout << hitpoints << std::endl << std::endl;
}