//
//  env.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/7/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <ncurses.h>

#include "env.h"
#include "../logger/logger.h"
#include "../character/character_store.h"
#include "../dungeon/dungeon.h"
#include "../events/event_queue.h"
#include "../parser/parser.h"
#include "../items/item_store.h"

int env_constants::USE_FOW    = 0;
int env_constants::USE_IPARSE = 1;
int env_constants::USE_MPARSE = 1;
int env_constants::USE_OBJ_DESC = 1; // use parsed objects by default
int env_constants::USE_MON_DESC = 1; // use parsed monsters by default
int env_constants::PARSE_ONLY_MODE = 0;
int env_constants::DEBUG_MODE = 0;
int env_constants::NCURSES_MODE = 0; // pc control enabled by default (so ncurses is enabled implicitly)
int env_constants::PC_AI_MODE = 0;
int env_constants::LOAD_DUNGEON = 0;
int env_constants::SAVE_DUNGEON = 0;
int env_constants::NUM_MONSTERS = 10;
int env_constants::QUIT_FLAG = 0;
int env_constants::STAIR_FLAG = 0;
char* env_constants::HOME = NULL;
char* env_constants::LOAD_FILE = NULL;
char* env_constants::SAVE_FILE = NULL;
uint8_t env_constants::X_START = 255;
uint8_t env_constants::Y_START = 255;

static int is_number(char* str);

static const char* help_text = "Usage: hexterm_havoc [options]\n\n"
                               "-a      , --ai          | Enable AI mode for PC (pc control used by default)\n"
                               "-h      , --help        | Print this help message.\n"
                               "-l<name>, --load=<name> | Load dungeon with name <name> (in save directory).\n"
                               "-m<val> , --nummon=<val>| Set the number of monsters in the dungeon\n"
                               "-n      , --ncurses     | Use Ncurses to render game\n"
                               "-p      , --parse-only  | Parse Objects and Monsters, then print them out\n"
                               "-s<name>, --save=<name> | Save the dungeon after loading/generating it with\n"
                               "                        |   name <name> (in save directory).\n"
                               "-x<val> , --xpos <val>  | Start the player at a specified x coord\n"
                               "-y<val> , --ypos <val>  | Start the player at a specified y coord\n";

void env::setup_environment() {
    logger::i("%%%% SETTING ENVIRONMENT %%%%");
    srand((unsigned)time(NULL));
    
    char* env;
    if((env = getenv("ENV"))) {
        if(!strcmp(env, "DEBUG")) {
            env_constants::DEBUG_MODE = 1;
        }
    }
    
#ifdef DEBUG
    env_constants::DEBUG_MODE = 1;
#endif // DEBUG
    
    if(!env_constants::PC_AI_MODE) {
        env_constants::NCURSES_MODE = 1; // pc control means we must use NCURSES_MODE
    }
    
    if(env_constants::PARSE_ONLY_MODE) {
        env_constants::NCURSES_MODE = 0;
    }
    
    if(env_constants::DEBUG_MODE) {
        logger::i("%%%% RUNNING IN DEBUG MODE %%%%");
    }
    
    if(env_constants::NCURSES_MODE) {
        logger::i("%%%% SETTING UP NCURSES %%%%");
        initscr();
        noecho();
        cbreak();
        set_escdelay(50);
        start_color();
        init_pair(COLOR_BLACK,   COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
        init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
        init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_CYAN,    COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_WHITE,   COLOR_BLACK,   COLOR_WHITE);
        keypad(stdscr, TRUE);
        mvprintw(0, 0, "DEBUG MODE");
        refresh();
    }
    
    logger::i("%%%% ENVIRONMENT SET %%%%");
}

void env::parse_args(int argc, char** argv) {
    // get the HOME env var
    char* env;
    if((env = getenv("HOME"))) {
        if(strlen(env) > 0) {
            env_constants::HOME = env;
        }
    }
    
    if(argc <= 1) return;
    
    int flag;
    int help_flag = 0;
    int e;
    int num;
    size_t size;
    
    opterr = 0;
    
    while(1) {
        // Setup options
        static struct option long_options[] = {
            {"ai",         no_argument,       0, 'a'},
            {"parse-only", no_argument,       0, 'p'},
            {"load",       optional_argument, 0, 'l'},
            {"save",       optional_argument, 0, 's'},
            {"nummon",     required_argument, 0, 'm'},
            {"ncurses",    no_argument,       0, 'n'},
            {"help",       no_argument,       0, 'h'},
            {"xpos",       required_argument, 0, 'x'},
            {"ypos",       required_argument, 0, 'y'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        flag = getopt_long(argc, argv, "pahnm:x:y:s::l::", long_options, &option_index);
        
        if(flag == -1) {
            break;
        }
        
        switch (flag) {
            case 0:
                if(long_options[option_index].flag != 0) {
                    break;
                }
                printf("option: %s", long_options[option_index].name);
                if(optarg) {
                    printf(" with args %s", optarg);
                }
                printf("\n");
                break;
                
            case 'a':
                env_constants::PC_AI_MODE = 1;
                break;
            
            case 'h':
                help_flag = 1;
                break;
                
            case 's':
                env_constants::SAVE_DUNGEON = 1;
                size = 1 + strlen(env_constants::HOME) + strlen("/.rlg327/") + ((optarg && *optarg) ? strlen(optarg) : strlen("dungeon"));
                env_constants::SAVE_FILE = (char*)calloc(size, sizeof(char));
                sprintf(env_constants::SAVE_FILE, "%s/.rlg327/", env_constants::HOME);
                e = mkdir(env_constants::SAVE_FILE, 0755);
                // clear the error if there is one.
                if(e == -1) {
                    errno = 0;
                } else {
                    logger::i("Creating save directory...");
                }
                strcat(env_constants::SAVE_FILE, (optarg && *optarg) ? optarg : "dungeon");
                logger::i("Save File Set: %s", env_constants::SAVE_FILE);
                break;
                
            case 'm':
                num = is_number(optarg);
                if(num < 1 || num > 50) {
                    logger::w("nummon is out of bounds! Must be in range [1, 50]");
                    fprintf(stderr, "Error: num monsters must be in range [1, 50]\n");
                    env::exit_gracefully();
                }
                env_constants::NUM_MONSTERS = num;
                break;
                
            case 'n':
                env_constants::NCURSES_MODE = 1;
                break;
                
            case 'p':
                env_constants::PARSE_ONLY_MODE = 1;
                break;
                
            case 'l':
                env_constants::LOAD_DUNGEON = 1;
                size = 1 + strlen(env_constants::HOME) + strlen("/.rlg327/") + ((optarg && *optarg) ? strlen(optarg) : strlen("dungeon"));
                env_constants::LOAD_FILE = (char*)calloc(size, sizeof(char));
                sprintf(env_constants::LOAD_FILE, "%s/.rlg327/", env_constants::HOME);
                e = mkdir(env_constants::LOAD_FILE, 0755);
                // clear the error if there is one.
                if(e == -1) {
                    errno = 0;
                } else {
                    logger::i("Creating save directory...");
                }
                strcat(env_constants::LOAD_FILE, (optarg && *optarg) ? optarg : "dungeon");
                logger::i("Load File Set: %s", env_constants::LOAD_FILE);
                break;
                
            case 'x':
                num = is_number(optarg);
                if(num < 1 || num > 79) {
                    logger::w("Input X is out of bounds! Must be in range [1, 79]");
                    fprintf(stderr, "Error: starting x coordinate must be in range [1, 79]\n");
                    env::exit_gracefully();
                }
                env_constants::X_START = num;
                break;
                
            case 'y':
                num = is_number(optarg);
                if(num < 1 || num > 20) {
                    logger::w("Input Y is out of bounds! Must be in range [1, 20]");
                    fprintf(stderr, "Error: starting y coordinate must be in range [1, 20]\n");
                    env::exit_gracefully();
                }
                env_constants::Y_START = num;
                break;
                
            case '?':
                break;
            
            default:
                env::exit_gracefully();
        }
    }
    
    if(opterr) {
        fprintf(stderr, "Error: invalid flag parsed.\n");
        env::exit_gracefully();
    }
    
    if(help_flag) {
        env::exit_gracefully();
    }
}

void env::exit_gracefully() {
    env::cleanup();
    printf("%s", help_text);
    exit(0);
}

void env::cleanup() {
    logger::i("Cleaning up environment...");
    if(env_constants::SAVE_FILE) {
        free(env_constants::SAVE_FILE);
    }
    if(env_constants::LOAD_FILE) {
        free(env_constants::LOAD_FILE);
    }
    item_store::teardown();
    character_store::teardown();
    event_queue::teardown();
    dungeon::teardown();
    parser::destroy_parser();
    
    if(env_constants::NCURSES_MODE) {
        endwin();
    }
    logger::i("Environment cleaned up");
}

void env::move_floors() {
    event_queue::move_floors();
    dungeon::move_floors();
    item_store::move_floors();
    character_store::move_floors();
    env_constants::STAIR_FLAG = 0; // reset flag
}

void env::print_status() {
    if(!env_constants::NCURSES_MODE) {
        return;
    }
    character* pc = character::get_pc();
    mvprintw(22, 1, "%3d/%3d", pc->hitpoints, 100);
}

static int is_number(char* str) {
    int val = atoi(str);
    return (val > 0 || (val == 0 && strlen(str) == 1)) ? val : -1;
}
