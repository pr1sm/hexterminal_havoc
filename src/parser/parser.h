//
//  parser.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include <string>

#include "monster_description.h"
#include "item_description.h"

#define default_monster_file "monster_desc.txt"
#define default_item_file "object_desc.txt"

#define PMNAME_CHECK  0x1
#define PMDESCS_CHECK 0x2
#define PMDESCE_CHECK 0x4
#define PMCOLOR_CHECK 0x8
#define PMSPEED_CHECK 0x10
#define PMABIL_CHECK  0x20
#define PMHP_CHECK    0x40
#define PMDAM_CHECK   0x80
#define PMSYMB_CHECK  0x100
#define PMCOMPLETE    0x1FF

#define PINAME_CHECK 0x1
#define PIDESCS_CHECK 0x2
#define PIDESCE_CHECK 0x4
#define PITYPE_CHECK 0x8
#define PICOLOR_CHECK 0x10
#define PIHIT_CHECK 0x20
#define PIDAM_CHECK 0x40
#define PIDODGE_CHECK 0x80
#define PIDEF_CHECK 0x100
#define PIWEIGHT_CHECK 0x200
#define PISPEED_CHECK 0x400
#define PIATTR_CHECK 0x800
#define PIVAL_CHECK 0x1000
#define PICOMPLETE 0x1FFF

class parser {
public:
    typedef enum parsed_type {
        pt_UNKNOWN = 0,
        // MON/ITEM TYPES
        pt_END,
        pt_NAME,
        pt_DESC,
        pt_DESC_END,
        pt_COLOR,
        pt_SPEED,
        pt_DAM,
        // MON ONLY TYPES
        pt_MSTART,
        pt_SYMB,
        pt_ABIL,
        pt_HP,
        // ITEM ONLY TYPES
        pt_ISTART,
        pt_TYPE,
        pt_HIT,
        pt_DODGE,
        pt_DEFENSE,
        pt_WEIGHT,
        pt_ATTR,
        pt_VAL
    } parsed_type;
    
    typedef enum parser_state {
        ps_UNKNOWN,
        ps_LOOKING,
        ps_PARSING
    } parser_state;
    
private:
    parsed_type parse_for_type(std::string line);
    void add_monster(monster_description* monster);
    void add_item(item_description* item);
public:
    
    monster_description** monster_list;
    int monster_size;
    int monster_len;
    
    item_description** item_list;
    int item_size;
    int item_len;
    
    parser();
    ~parser();
    
    // 0 - success
    // 1 - description file not found
    // 2 - file is not correct type
    int parse_monsters();
    
    // 0 - success
    // 1 - description file not found
    // 2 - file is not correct type
    int parse_items();
    
    void print_monsters();
    void print_items();
};

#endif /* parser_h */
