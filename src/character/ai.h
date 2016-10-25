//
//  ai.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/16/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef ai_h
#define ai_h

#ifdef __cplusplus
    #include "character.h"
#else
    #include "character_t.h"
#endif // __cplusplus
#include "../graph/graph.h"
#include "../point/point.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern graph_t* _PLAYER_PATH;

void setup_pc_movement();
void handle_pc_move();

void handle_npc_move(character_t* c);
path_node_t* los_to_pc(point_t* p);
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* ai_h */
