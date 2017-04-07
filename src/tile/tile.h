//
//  tile.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/31/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef tile_h
#define tile_h

#include "../point/point.h"
#include "../env/env.h"

#define PM_DUNGEON 0
#define PM_ROOM_PATH_MAP 1
#define PM_TUNN_PATH_MAP 2

typedef enum {
    tc_UNSET, tc_BORDER, tc_ROCK, tc_ROOM, tc_PATH, tc_UPSTR, tc_DNSTR
} tile_content;

class tile {
private:
    point* change_location;
    uint8_t change_rock_hardness;
    uint8_t change_dist;
    uint8_t change_dist_tunnel;
    char change_last_known_content;
public:
    point* location;
    uint8_t rock_hardness;
    uint8_t dist;
    uint8_t dist_tunnel;
    tile_content content;
    tile_content change_content;
    char last_known_content;
    
    tile(uint8_t x, uint8_t y);
    tile(uint8_t value, bool room);
    ~tile();
    
    void update_hardness(uint8_t value);
    void update_content(tile_content value);
    void update_dist(uint8_t value);
    void update_dist_tunnel(uint8_t value);
    void propose_update_hardness(uint8_t value);
    void propose_update_content(tile_content value);
    void propose_update_dist(uint8_t value);
    void propose_update_dist_tunnel(uint8_t value);
    void commit_updates();
    int  are_changes_proposed();
    char char_for_content(int mode);
    uint8_t npc_tunnel();
    
};

#endif /* tile_h */
