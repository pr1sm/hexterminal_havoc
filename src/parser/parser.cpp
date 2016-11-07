//
//  parser.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "parser.h"
#include "monster_description.h"
#include "../logger/logger.h"

parser::parser() {
    monster_len = 0;
    monster_size = 0;
    monster_list = NULL;
}

parser::~parser() {
    int i;
    if(monster_list != NULL) {
        for(i = 0; i < monster_size; i++) {
            if(monster_list[i] != NULL) {
                delete monster_list[i];
            }
        }
        free(monster_list);
    }
}

int parser::parse_monsters() {
    std::ifstream fin;
    std::string file_name(env_constants::HOME);
    file_name.append("/.rlg327/");
    file_name.append(default_monster_file);
    fin.open(file_name.c_str());
    if(!fin.good()) {
        logger::e("Monster Description file could not be found!");
        return 1;
    }
    
    // check first line for description
    std::string line;
    std::getline(fin, line);
    
    if(line.compare("RLG327 MONSTER DESCRIPTION 1") != 0) {
        logger::e("Monster Description file is not the correct format!");
        return 2;
    }
    
    monster_description* mon = NULL;
    parser_state state = ps_LOOKING;
    
    // read each line of the file
    while (!fin.eof())
    {
        line.clear();
        std::getline(fin, line);
        
        parsed_type type = parse_for_type(line);
        
        if(state == ps_LOOKING && type == pt_START) {
            // we found one, start looking
            state = ps_PARSING;
            if(mon != NULL) {
                delete mon;
            }
            mon = new monster_description();
            mon->check = 0;
            continue;
        } else if(state == ps_PARSING && mon == NULL) {
            // we are in an error, reset
            state = ps_LOOKING;
        } else if(state == ps_PARSING) {
            // we are parsing and mon is setup, check the type
            
            if((mon->check & (PDESCS_CHECK | PDESCE_CHECK)) == PDESCS_CHECK) {
                // description has started, check for unknown or end
                if(type == pt_DESC_END) {
                    mon->check |= PDESCE_CHECK;
                    mon->parse_desc(mon->raw_desc);
                } else if(type == pt_UNKNOWN) {
                    mon->raw_desc.append("\n");
                    mon->raw_desc.append(line);
                } else {
                    // error another type was found during description parsing
                    std::cout << "parsing error, starting over" << std::endl;
                    delete mon;
                    mon = NULL;
                    state = ps_LOOKING;
                }
            } else if(type == pt_NAME) {
                mon->parse_name(line);
                mon->check |= PNAME_CHECK;
            } else if(type == pt_SYMB) {
                mon->parse_symb(line);
                mon->check |= PSYMB_CHECK;
            } else if(type == pt_DESC) {
                mon->raw_desc = line;
                mon->check |= PDESCS_CHECK;
            } else if(type == pt_DESC_END) {
                mon->check |= PDESCE_CHECK;
            } else if(type == pt_COLOR) {
                mon->parse_color(line);
                mon->check |= PCOLOR_CHECK;
            } else if(type == pt_SPEED) {
                mon->parse_speed(line);
                mon->check |= PSPEED_CHECK;
            } else if(type == pt_ABIL) {
                mon->parse_attrs(line);
                mon->check |= PABIL_CHECK;
            } else if(type == pt_HP) {
                mon->parse_hp(line);
                mon->check |= PHP_CHECK;
            } else if(type == pt_DAM) {
                mon->parse_damage(line);
                mon->check |= PDAM_CHECK;
            } else if(type == pt_END) {
                if(mon->check != PCOMPLETE) {
                    // error
                    logger::e("we had an error parsing this monster, deleting it and starting over!");
                    delete mon;
                    mon = NULL;
                    state = ps_LOOKING;
                } else {
                    add_monster(mon);
                    mon = NULL;
                    state = ps_LOOKING;
                }
            }
        }
        
    }
    
    fin.close();
    
    int i;
    std::cout << "Found " << monster_len << " monsters" << std::endl << std::endl;
    for(i = 0; i < monster_len; i++) {
        monster_list[i]->print();
    }
    
    return 0;
}

parser::parsed_type parser::parse_for_type(std::string line) {
    if(line.compare(0, 13, "BEGIN MONSTER") == 0) {
        return pt_START;
    } else if(line.compare(0, 4, "NAME") == 0) {
        return pt_NAME;
    } else if(line.compare(0, 4, "SYMB") == 0) {
        return pt_SYMB;
    } else if(line.compare(0, 5, "COLOR") == 0) {
        return pt_COLOR;
    } else if(line.compare(0, 4, "DESC") == 0) {
        return pt_DESC;
    } else if(line.compare(".") == 0) {
        return pt_DESC_END;
    } else if(line.compare(0, 3, "DAM") == 0) {
        return pt_DAM;
    } else if(line.compare(0, 5, "SPEED") == 0) {
        return pt_SPEED;
    } else if(line.compare(0, 2, "HP") == 0) {
        return pt_HP;
    } else if(line.compare(0, 4, "ABIL") == 0) {
        return pt_ABIL;
    } else if(line.compare(0, 3, "END") == 0) {
        return pt_END;
    }
    
    return pt_UNKNOWN;
}

void parser::add_monster(monster_description* monster) {
    if(monster == NULL) {
        logger::w("Attempt to add NULL monster to list");
    }
    if(monster_len > monster_size-1) {
        int new_size = monster_size == 0 ? 4 : monster_size*2;
        // realloc monster list;
        monster_description** new_list = (monster_description**)realloc(monster_list, new_size*sizeof(*monster_list));
        if(new_list == NULL) {
            // DEAL WITH ERROR!
            logger::e("Could not allocate enough space to add monster! monster not added");
            delete monster;
            return;
        }
        int i;
        for(i = monster_size; i < new_size; i++) {
            new_list[i] = NULL;
        }
        monster_list = new_list;
        monster_size = new_size;
    }
    
    monster_list[monster_len++] = monster;
}