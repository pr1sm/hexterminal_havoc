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

class monster_description {
public:
    std::string name;
    std::string symb;
    std::string desc;
    std::string color;
    std::string speed;
    std::string abilities;
    std::string hitpoints;
    std::string damage;
    int check;
    
    void print();
};


#endif /* monster_description_h */
