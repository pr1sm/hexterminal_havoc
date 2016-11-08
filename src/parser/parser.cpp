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
    
    item_len = 0;
    item_size = 0;
    item_list = NULL;
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
    if(item_list != NULL) {
        for(i = 0; i < item_size; i++) {
            if(item_list[i] != NULL) {
                delete item_list[i];
            }
        }
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
        
        if(state == ps_LOOKING && type == pt_MSTART) {
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
            
            if((mon->check & (PMDESCS_CHECK | PMDESCE_CHECK)) == PMDESCS_CHECK) {
                // description has started, check for unknown or end
                if(type == pt_DESC_END) {
                    mon->check |= PMDESCE_CHECK;
                    mon->parse_desc(mon->raw_desc);
                } else if(type == pt_UNKNOWN) {
                    mon->raw_desc.append("\n");
                    mon->raw_desc.append(line);
                } else {
                    // error another type was found during description parsing
                    logger::e("found another type during desc parsing: %d, starting over", type);
                    delete mon;
                    mon = NULL;
                    state = ps_LOOKING;
                }
            } else if(type == pt_NAME) {
                mon->parse_name(line);
                mon->check |= PMNAME_CHECK;
            } else if(type == pt_SYMB) {
                mon->parse_symb(line);
                mon->check |= PMSYMB_CHECK;
            } else if(type == pt_DESC) {
                mon->raw_desc = line;
                mon->check |= PMDESCS_CHECK;
            } else if(type == pt_DESC_END) {
                mon->check |= PMDESCE_CHECK;
            } else if(type == pt_COLOR) {
                mon->parse_color(line);
                mon->check |= PMCOLOR_CHECK;
            } else if(type == pt_SPEED) {
                mon->parse_speed(line);
                mon->check |= PMSPEED_CHECK;
            } else if(type == pt_ABIL) {
                mon->parse_attrs(line);
                mon->check |= PMABIL_CHECK;
            } else if(type == pt_HP) {
                mon->parse_hp(line);
                mon->check |= PMHP_CHECK;
            } else if(type == pt_DAM) {
                mon->parse_damage(line);
                mon->check |= PMDAM_CHECK;
            } else if(type == pt_END) {
                if(mon->check != PMCOMPLETE) {
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
            } else {
                // We have another type (from item description)
                // this is an error, so we need to start over
                logger::e("bad type found during parsing: %d.  Restarting search...", type);
                state = ps_LOOKING;
            }
        }
        
    }
    
    fin.close();
    
    return 0;
}

void parser::print_monsters() {
    int i;
    std::cout << "Found " << monster_len << " monsters" << std::endl << std::endl;
    for(i = 0; i < monster_len; i++) {
        monster_list[i]->print();
    }
}

int parser::parse_items() {
    std::ifstream fin;
    std::string file_name(env_constants::HOME);
    file_name.append("/.rlg327/");
    file_name.append(default_item_file);
    fin.open(file_name.c_str());
    if(!fin.good()) {
        logger::e("Monster Description file could not be found!");
        return 1;
    }
    
    // check first line for description
    std::string line;
    std::getline(fin, line);
    
    if(line.compare("RLG327 OBJECT DESCRIPTION 1") != 0) {
        logger::e("Monster Description file is not the correct format!");
        return 2;
    }
    
    item_description* item = NULL;
    parser_state state = ps_LOOKING;
    
    while(!fin.eof()) {
        line.clear();
        std::getline(fin, line);
        
        parsed_type type = parse_for_type(line);
        
        if(state == ps_LOOKING && type == pt_ISTART) {
            state = ps_PARSING;
            if(item != NULL) {
                delete item;
            }
            item = new item_description();
            item->check = 0;
            continue;
        } else if(state == ps_PARSING && item == NULL) {
            state = ps_LOOKING;
        } else if(state == ps_PARSING) {
            // we are parsing and item is not NULL, check the type
            
            if((item->check & (PIDESCS_CHECK | PIDESCE_CHECK)) == PIDESCS_CHECK) {
                // description started, check for unknown or end
                if(type == pt_DESC_END) {
                    item->check |= PIDESCE_CHECK;
                    item->parse_desc(item->raw_desc);
                } else if(type == pt_UNKNOWN) {
                    item->raw_desc.append("\n");
                    item->raw_desc.append(line);
                } else {
                    // error another type was found during description parsing
                    logger::e("found another type during desc parsing: %d, starting over", type);
                    delete item;
                    item = NULL;
                    state = ps_LOOKING;
                }
            } else if(type == pt_NAME) {
                item->parse_name(line);
                item->check |= PINAME_CHECK;
            } else if(type == pt_TYPE) {
                item->parse_type(line);
                item->check |= PITYPE_CHECK;
            } else if(type == pt_DESC) {
                item->raw_desc = line;
                item->check |= PIDESCS_CHECK;
            } else if(type == pt_DESC_END) {
                item->check |= PIDESCE_CHECK;
            } else if(type == pt_COLOR) {
                item->parse_color(line);
                item->check |= PICOLOR_CHECK;
            } else if(type == pt_HIT) {
                item->parse_hit(line);
                item->check |= PIHIT_CHECK;
            } else if(type == pt_DAM) {
                item->parse_damage(line);
                item->check |= PIDAM_CHECK;
            } else if(type == pt_DODGE) {
                item->parse_dodge(line);
                item->check |= PIDODGE_CHECK;
            } else if(type == pt_DEFENSE) {
                item->parse_defense(line);
                item->check |= PIDEF_CHECK;
            } else if(type == pt_WEIGHT) {
                item->parse_weight(line);
                item->check |= PIWEIGHT_CHECK;
            } else if(type == pt_SPEED) {
                item->parse_speed(line);
                item->check |= PISPEED_CHECK;
            } else if(type == pt_ATTR) {
                item->parse_attr(line);
                item->check |= PIATTR_CHECK;
            } else if(type == pt_VAL) {
                item->parse_value(line);
                item->check |= PIVAL_CHECK;
            } else if(type == pt_END) {
                if(item->check != PICOMPLETE) {
                    // error
                    logger::e("we had an error parsing this item, deleting it and starting over!");
                    delete item;
                    item = NULL;
                    state = ps_LOOKING;
                } else {
                    add_item(item);
                    item = NULL;
                    state = ps_LOOKING;
                }
            } else {
                // We have another type (from monster description)
                // this is an error, so we need to start over
                logger::e("bad type found during parsing: %d.  Restarting search...", type);
                state = ps_LOOKING;
            }
        }
    }
    
    fin.close();
    
    return 0;
}

void parser::print_items() {
    int i;
    std::cout << "Found " << item_len << " items" << std::endl << std::endl;
    for(i = 0; i < item_len; i++) {
        item_list[i]->print();
    }
}

parser::parsed_type parser::parse_for_type(std::string line) {
    if(line.compare(0, 13, "BEGIN MONSTER") == 0) {
        return pt_MSTART;
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
    } else if(line.compare(0, 12, "BEGIN OBJECT") == 0) {
        return pt_ISTART;
    } else if(line.compare(0, 4, "TYPE") == 0) {
        return pt_TYPE;
    } else if(line.compare(0, 3, "HIT") == 0) {
        return pt_HIT;
    } else if(line.compare(0, 5, "DODGE") == 0) {
        return pt_DODGE;
    } else if(line.compare(0, 3, "DEF") == 0) {
        return pt_DEFENSE;
    } else if(line.compare(0, 6, "WEIGHT") == 0) {
        return pt_WEIGHT;
    } else if(line.compare(0, 4, "ATTR") == 0) {
        return pt_ATTR;
    } else if(line.compare(0, 3, "VAL") == 0) {
        return pt_VAL;
    }
    
    return pt_UNKNOWN;
}

void parser::add_monster(monster_description* monster) {
    if(monster == NULL) {
        logger::w("Attempt to add NULL monster to list");
        return;
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

void parser::add_item(item_description* item) {
    if(item == NULL) {
        logger::w("Attempt to add NULL item to list");
        return;
    }
    if(monster_len > monster_size-1) {
        int new_size = monster_size == 0 ? 4 : monster_size*2;
        // realloc item list
        item_description** new_list = (item_description**)realloc(item_list, new_size*sizeof(*item_list));
        if(new_list == NULL) {
            // DEAL WITH ERROR!
            logger::e("Could not allocate enough space to add item! item not added");
            delete item;
            return;
        }
        int i;
        for(i = item_size; i < new_size; i++) {
            new_list[i] = NULL;
        }
        item_list = new_list;
        item_size = new_size;
    }
    
    item_list[item_len++] = item;
}