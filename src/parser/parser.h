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

#define default_monster_file "monster_desc.txt"

#define PNAME_CHECK  0x1
#define PDESCS_CHECK 0x2
#define PDESCE_CHECK 0x4
#define PCOLOR_CHECK 0x8
#define PSPEED_CHECK 0x10
#define PABIL_CHECK  0x20
#define PHP_CHECK    0x40
#define PDAM_CHECK   0x80
#define PSYMB_CHECK  0x100
#define PCOMPLETE    0x1FF

class parser {
public:
    typedef enum parsed_type {
        pt_UNKNOWN,
        pt_START,
        pt_END,
        pt_NAME,
        pt_SYMB,
        pt_DESC,
        pt_DESC_END,
        pt_COLOR,
        pt_SPEED,
        pt_ABIL,
        pt_HP,
        pt_DAM
    } parsed_type;
    
    typedef enum parser_state {
        ps_UNKNOWN,
        ps_LOOKING,
        ps_PARSING
    } parser_state;
    
private:
    parsed_type parse_for_type(std::string line);
    void add_monster(monster_description* monster);
public:
    
    monster_description** monster_list;
    int monster_size;
    int monster_len;
    
    parser();
    ~parser();
    
    // 0 - success
    // 1 - description file not found
    // 2 - file is not correct type
    int parse_monsters();
};

#endif /* parser_h */
