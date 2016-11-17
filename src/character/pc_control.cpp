//
//  pc_control.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <ncurses.h>
#include <string>
#include <sstream>

#include "pc_control.h"
#include "character_store.h"
#include "../events/event_queue.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../items/item_store.h"
#include "../items/item.h"
#include "../parser/item_description.h"

void pc_control::setup_control_movement() {
    event_queue::add_event(character::get_pc());
}

void pc_control::handle_control_move() {
    character** characters = character_store::get_characters();
    dungeon* d = dungeon::get_dungeon();
    character* pc;
    point* pc_pos;
    point* npc_pos;
    pc = character::get_pc();
    pc_pos = pc->position;
    pc_move_t move = mv_NONE;
    point* dest = new point(pc_pos);
    int i;
    mvprintw(0, 0, "ENTER COMMAND:                                ");
    refresh();
    int is_valid = 0;
    do {
        move = mv_NONE;
        dest->x = pc_pos->x;
        dest->y = pc_pos->y;
        int ch = getch();
        switch (ch) {
            case PC_QUIT:
                env_constants::QUIT_FLAG = 1;
                is_valid = 1;
                break;
                
            case PC_UP_8:
            case PC_UP_k:
                move = mv_UP;
                break;
                
            case PC_UP_LEFT_7:
            case PC_UP_LEFT_y:
                move = mv_UL;
                break;
                
            case PC_UP_RIGHT_9:
            case PC_UP_RIGHT_u:
                move = mv_UR;
                break;
                
            case PC_LEFT_4:
            case PC_LEFT_h:
                move = mv_LT;
                break;
                
            case PC_RIGHT_6:
            case PC_RIGHT_l:
                move = mv_RT;
                break;
                
            case PC_DOWN_LEFT_1:
            case PC_DOWN_LEFT_b:
                move = mv_DL;
                break;
                
            case PC_DOWN_2:
            case PC_DOWN_j:
                move = mv_DN;
                break;
                
            case PC_DOWN_RIGHT_3:
            case PC_DOWN_RIGHT_n:
                move = mv_DR;
                break;
                
            case PC_REST_5:
            case PC_REST_SPACE:
                move = mv_RS;
                break;
                
            case PC_MONSTER_LIST:
                move = mv_ML;
                is_valid = 1;
                break;
                
            case PC_DOWNSTAIRS:
                move = mv_DNSTR;
                break;
                
            case PC_UPSTAIRS:
                move = mv_UPSTR;
                break;
                
            case PC_INV_LIST:
                move = mv_IL;
                is_valid = 1;
                break;
                
            case PC_EQP_LIST:
                move = mv_EL;
                is_valid = 1;
                break;
            
            case PC_INV_INSPECT:
                move = mv_ILI;
                is_valid = 1;
                break;
                
            case PC_EQUIP:
                move = mv_EQP;
                is_valid = 1;
                break;
                
            case PC_UNEQUIP:
                move = mv_UEQP;
                is_valid = 1;
                break;
                
            case PC_DROP:
                move = mv_DRP;
                is_valid = 1;
                break;
                
            case PC_EXPUNGE:
                move = mv_EX;
                is_valid = 1;
                break;
                
            default:
                mvprintw(0, 0, "INVALID COMMAND: %3d                            ", ch);
                refresh();
                break;
        }
        
        if(!is_valid && move == mv_UPSTR) {
            if(d->tiles[pc_pos->y][pc_pos->x]->content == tc_UPSTR) {
                is_valid = 1;
            }
            
            if(!is_valid) {
                mvprintw(0, 0, "Whoops! You can't go up without stairs!");
            }
        } else if(!is_valid && move == mv_DNSTR) {
            if(d->tiles[pc_pos->y][pc_pos->x]->content == tc_DNSTR) {
                is_valid = 1;
            }
            
            if(!is_valid) {
                mvprintw(0, 0, "Whoops! You can't go down without stairs!");
            }
        } else if(!is_valid && move != mv_NONE) {
            if(move == mv_UL || move == mv_UP || move == mv_UR) {
                dest->y -= 1;
            } else if(move == mv_DL || move == mv_DN || move == mv_DR) {
                dest->y += 1;
            }
            if(move == mv_UL || move == mv_LT || move == mv_DL) {
                dest->x -= 1;
            } else if(move == mv_UR || move == mv_RT || move == mv_DR) {
                dest->x += 1;
            }
            
            if(dest->x < 1 || dest->x > DUNGEON_WIDTH-2 || dest->y < 1 || dest->y > DUNGEON_HEIGHT-2) {
                is_valid = 0;
            } else if(d->tiles[dest->y][dest->x]->content != tc_ROCK) {
                is_valid = 1;
            }
            
            if(!is_valid) {
                mvprintw(0, 0, "Whoops! You can't move there, try again");
                refresh();
            }
        }
        if(move == mv_ML) {
            character_store::start_monster_list();
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_IL) {
            pc_control::show_inventory(PC_INV_LIST);
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_EL) {
            pc_control::show_equipment(PC_EQP_LIST);
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_ILI) {
            pc_control::show_inventory(PC_INV_INSPECT);
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_EQP) {
            pc_control::show_inventory(PC_EQUIP);
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_UEQP) {
            pc_control::show_equipment(PC_UNEQUIP);
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_DRP) {
            pc_control::show_inventory(PC_DROP);
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        } else if(move == mv_EX) {
            pc_control::show_inventory(PC_EXPUNGE);
            dungeon::get_dungeon()->print(PM_DUNGEON);
            mvprintw(0, 0, "ENTER COMMAND:                          ");
            refresh();
            is_valid = 0;
        }
    } while(!is_valid);
    
    if(move == mv_ML) {
        logger::i("Starting monster list...");
        character_store::start_monster_list();
        logger::i("Ending monster list");
    } else if(move == mv_UPSTR) {
        logger::i("Moving upstairs");
        env_constants::STAIR_FLAG = 1; // move upstairs
    } else if(move == mv_DNSTR) {
        logger::i("Moving downstairs");
        env_constants::STAIR_FLAG = 2; // move downstairs
    } else if(move != mv_RS) {
        logger::i("Moving to point (%2d, %2d)", dest->x, dest->y);
        // move pc
        pc->set_position(dest);
        pc_pos = pc->position;
        
        // check for collision
        for(i = 0; i < character_store::CHARACTER_COUNT; i++) {
            npc_pos = characters[i]->position;
            if(pc_pos->distance_to(npc_pos) == 0) {
                characters[i]->is_dead = 1;
            }
            characters[i]->is_seen = (pc_pos->distance_to(npc_pos) <= 3);
        }
    } else if(move == mv_RS) {
        logger::i("Resting 1 turn");
    }
    
    // check items
    if(item_store::contains_item(pc->position)) {
        item_store::pickup_item(pc->position);
    }
    
    delete dest;
    event_queue::add_event(pc);
}

void pc_control::show_inventory(int mode) {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    print_inventory(mode);
    character* pc = character::get_pc();
    int next_cmd = 0;
    do {
        next_cmd = getch();
        if(pc->inventory_size > 0) {
            if(mode == PC_EQUIP && (next_cmd >= 48 && next_cmd <= 57)) {
                item* itm = pc->inventory[next_cmd-48];
                if(itm == NULL) {
                    // print error message
                } else {
                    int res = character::pc_equip_item(itm);
                    if(res != 0) {
                        // print error message
                    }
                }
                print_inventory(mode);
            } else if(mode == PC_INV_INSPECT && (next_cmd >= 48 && next_cmd <= 57)) {
                print_detailed_item(next_cmd-48);
                print_inventory(mode);
            } else if(mode == PC_DROP && (next_cmd >= 48 && next_cmd <= 57)) {
                item* itm = pc->inventory[next_cmd-48];
                if(itm == NULL) {
                    // print error message
                } else {
                    int res = character::pc_drop_item(itm);
                    if(res != 0) {
                        // print error message
                    }
                }
                print_inventory(mode);
            } else if(mode == PC_EXPUNGE && (next_cmd >= 48 && next_cmd <= 57)) {
                item* itm = pc->inventory[next_cmd-48];
                if(itm == NULL) {
                    // print error message
                } else {
                    int res = character::pc_expunge_item(itm);
                    if(res != 0) {
                        // print error message
                    }
                }
                print_inventory(mode);
            }
        }
    } while(next_cmd != PC_ML_CLOSE);
}

void pc_control::print_inventory(int mode) {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    character* pc = character::get_pc();
    int i;
    clear();
    mvprintw(20, 1, "Press ESC to close...");
    if(pc->inventory_size > 0) {
        mvprintw(1, 1, "Inventory List");
        for(i = 0; i < pc->inventory_size; i++) {
            item* item = pc->inventory[i];
            if(item == NULL) {
                mvprintw(i+2, 3, "%d) EMPTY", i);
            } else if(item->state == is_picked_up) {
                attron(COLOR_PAIR(item->color));
                mvprintw(i+2, 3, "%d) %c - %s (sp: %d, dmg: %s)", i, item->symb, item->name.c_str(), item->speed, item->damage->to_string().c_str());
                attroff(COLOR_PAIR(item->color));
            } else {
                mvprintw(i+2, 3, "%d) EMPTY", i);
            }
        }
        if(mode == PC_EQUIP) {
            mvprintw(0, 0, "Enter the item [0-9] you want to equip: ");
        } else if(mode == PC_INV_INSPECT) {
            mvprintw(0, 0, "Enter the item [0-9] you want to inspect: ");
        } else if(mode == PC_DROP) {
            mvprintw(0, 0, "Enter the item [0-9] you want to drop: ");
        } else if(mode == PC_EXPUNGE) {
            mvprintw(0, 0, "Enter the item [0-9] you want to delete: ");
        }
    } else {
        mvprintw(1, 1, "Inventory List - EMPTY! (pickup some items around the dungeon!)");
    }
    refresh();
}

void pc_control::show_equipment(int mode) {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    character* pc = character::get_pc();
    print_equipment(mode);
    refresh();
    int next_cmd = 0;
    do {
        next_cmd = getch();
        if(mode == PC_UNEQUIP && (next_cmd >= 97 && next_cmd <= 108)) {
            item* itm = pc->equipment->equipped_items()[next_cmd-97];
            int res = character::pc_unequip_item(itm);
            if(res == 0) {
                // print success
            }
            print_equipment(mode);
        }
    } while(next_cmd != PC_ML_CLOSE);
}

void pc_control::print_equipment(int mode) {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    character* pc = character::get_pc();
    int i;
    clear();
    mvprintw(20, 1, "Press ESC to close...");
    item** equipped_items = pc->equipment->equipped_items();
    if(equipped_items != NULL) {
        mvprintw(1, 1, "Equipment List");
        for(i = 0; i < pc->equipment->equip_list_length; i++) {
            item* item = equipped_items[i];
            if(item == NULL) {
                mvprintw(i+2, 3, "%c) EMPTY", 'a'+i);
            } else if(item->state == is_equipped) {
                attron(COLOR_PAIR(item->color));
                mvprintw(i+2, 3, "%c) %c - %s (sp: %d, dmg: %s)", 'a'+i, item->symb, item->name.c_str(), item->speed, item->damage->to_string().c_str());
                attroff(COLOR_PAIR(item->color));
            } else {
                mvprintw(i+2, 3, "%c) EMPTY", 'a'+i);
            }
        }
        if(mode == PC_UNEQUIP) {
            mvprintw(0, 0, "Enter the item [a-l] you want to unequip: ");
        }
    } else {
        mvprintw(1, 1, "Equipment List - EMPTY! (pickup some items around the dungeon and equip them!)");
    }
    refresh();
}

void pc_control::print_detailed_item(int index) {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    
    character* pc = character::get_pc();
    if(index < 0 || index >= pc->inventory_len) {
        return;
    }
    
    clear();
    item* itm = pc->inventory[index];
    attron(COLOR_PAIR(itm->color));
    mvprintw(2, 1, "Name      ~> %s", itm->name.c_str());
    mvprintw(3, 1, "Type      ~> %s | Symb ~> %c", item_description::print_type(itm->type).c_str(), itm->symb);
    mvprintw(4, 1, "Damage    ~> %s", itm->damage->to_string().c_str());
    mvprintw(5, 1, "Weight    ~> %5d | Dodge   ~> %5d", itm->weight, itm->dodge);
    mvprintw(6, 1, "Defense   ~> %5d | Value   ~> %5d", itm->defense, itm->value);
    mvprintw(7, 1, "Hitpoints ~> %5d | Speed   ~> %5d", itm->hit, itm->speed);
    mvprintw(8, 1, "Description:");
    std::stringstream ss(itm->desc);
    std::string line;
    int i = 9;
    while(std::getline(ss, line, '\n') && i < 20) {
        mvprintw(i++, 1, line.c_str());
    }
    attroff(COLOR_PAIR(itm->color));
    mvprintw(20, 1, "Press ESC to close...");
    refresh();
    int next_cmd = 0;
    do {
        next_cmd = getch();
    } while(next_cmd != PC_ML_CLOSE);
}
