//
//  ai.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef ai_h
#define ai_h

#include "character.h"
#include "../graph/graph.h"
#include "../point/point.h"

class ai {
private:
    static void get_random_dir(character* c, point* p);
public:
    static graph* PLAYER_PATH;
    
    static void setup_pc_movement();
    static void handle_pc_move();
    static void handle_npc_move(character* c);
    static path_node* los_to_pc(point* p);
    
};

#endif /* ai_h */
