//
//  dice.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef dice_h
#define dice_h

#include <stdio.h>
#include <string>

class dice {
public:
    int base;
    int num_dice;
    int sides;
    
    dice(std::string str);
    
    std::string to_string();
};

#endif /* dice_h */
