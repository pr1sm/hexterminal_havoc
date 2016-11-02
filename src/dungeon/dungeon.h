//
//  dungeon.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef dungeon_h
#define dungeon_h

#include "../tile/tile.h"
#include "../point/point.h"
#include "../env/env.h"
#include "../room/room.h"
#include "../graph/graph.h"

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80
#define ROCK_MAX 255
#define ROCK_HARD 171
#define ROCK_MED  85
#define ROCK_SOFT 1

class dungeon {
private:
    static dungeon* _base;
    
    static void     d_log_room(room* r);
    
    void accent();
    void diffuse();
    void smooth();
    void add_borders();
    bool is_open_space();
    void add_rooms();
    void write_pgm(const char* file_name, int zone);
    void generate_terrain();
    void place_rooms();
    void place_staircases();
    void pathfind();
    void update_path_hardnesses();
    
    dungeon();
    ~dungeon();
public:
    tile*** tiles;
    room** rooms;
    int    room_size;
    graph* tunnel_map;
    graph* non_tunnel_map;
    
    void update_path_maps();
    void print(int mode);
    void printn(int mode);
    void load();
    void save();
    void generate();
    
    static dungeon* get_dungeon();
    static void     teardown();
    static dungeon* move_floors();
    static void     rand_point(dungeon* d, point* p);
};

#endif /* dungeon_h */
