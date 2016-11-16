//
//  character_store.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef character_store_h
#define character_store_h

#include "character.h"
#include "../point/point.h"

class character_store {
private:
    static character** _characters;
    static character_id_t* _alive_characters;
    static int _characters_size;
    static int _characters_count;
    static void print_char(character* npc);
    static void setup_npc(character* npc);
    
    static character* gen_npc(bool use_descriptor);
    
public:
    static int CHARACTER_COUNT;
    
    static void setup();
    static void teardown();
    static int  contains_npc(point* p);
    static char get_char_for_npc_at_index(int i, int mode);
    static character** get_characters();
    static character_id_t* get_alive_characters();
    static character* npc_for_id(character_id_t id);
    static int  is_finished();
    static void npc_cleanup();
    static void start_monster_list();
    static void move_floors();
};

#endif /* character_store_h */
