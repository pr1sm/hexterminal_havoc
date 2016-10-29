//
//  env_flags.c
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
#include <sys/stat.h>
#include <ncurses.h>

#include "env.h"
#include "../logger/logger.h"
#include "../character/character_store.h"
#include "../dungeon/dungeon.h"
#include "../events/event_queue.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int DEBUG_MODE   = 0;
int NCURSES_MODE = 0;
int PC_AI_MODE   = 0; // pc control enabled by default (so ncurses is enabled implicitly)
int LOAD_DUNGEON = 0;
int SAVE_DUNGEON = 0;
int NUM_MONSTERS = 10; // default is 10
int QUIT_FLAG    = 0;
int STAIR_FLAG   = 0;
uint8_t X_START  = 255;
uint8_t Y_START  = 255;
char* HOME;
char* LOAD_FILE;
char* SAVE_FILE;

static int is_number(char* str);

static const char* help_text = "Usage: hexterm_havoc [options]\n\n"
                               "-a      , --ai          | Enable AI mode for PC (pc control used by default)\n"
                               "-h      , --help        | Print this help message.\n"
                               "-l<name>, --load=<name> | Load dungeon with name <name> (in save directory).\n"
                               "-m<val> , --nummon=<val>| Set the number of monsters in the dungeon\n"
                               "-n      , --ncurses     | Use Ncurses to render game\n"
                               "-s<name>, --save=<name> | Save the dungeon after loading/generating it with\n"
                               "                        |   name <name> (in save directory).\n"
                               "-x<val> , --xpos <val>  | Start the player at a specified x coord\n"
                               "-y<val> , --ypos <val>  | Start the player at a specified y coord\n";

static void setup_environment() {
    logger.i("%%%% SETTING ENVIRONMENT %%%%");
    char* env;
    if((env = getenv("ENV"))) {
        if(!strcmp(env, "DEBUG")) {
            DEBUG_MODE = 1;
        }
    }
    
    if((env = getenv("HOME"))) {
        if(strlen(env) > 0) {
            HOME = env;
        }
    }
    
#ifdef DEBUG
    DEBUG_MODE = 1;
#endif // DEBUG
    
    if(!PC_AI_MODE) {
        NCURSES_MODE = 1; // pc control means we must use NCURSES_MODE
    }
    
    if(DEBUG_MODE) {
        logger.i("%%%% RUNNING IN DEBUG MODE %%%%");
    }
    
    if(NCURSES_MODE) {
        logger.i("%%%% SETTING UP NCURSES %%%%");
        initscr();
        noecho();
        cbreak();
        set_escdelay(50);
        keypad(stdscr, TRUE);
        mvprintw(0, 0, "DEBUG MODE");
        refresh();
    }
    
    logger.i("%%%% ENVIRONMENT SET %%%%");
}

static void parse_args(int argc, char** argv) {
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
            {"ai",      no_argument,       0, 'a'},
            {"load",    optional_argument, 0, 'l'},
            {"save",    optional_argument, 0, 's'},
            {"nummon",  required_argument, 0, 'm'},
            {"ncurses", no_argument,       0, 'n'},
            {"help",    no_argument,       0, 'h'},
            {"xpos",    required_argument, 0, 'x'},
            {"ypos",    required_argument, 0, 'y'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        flag = getopt_long(argc, argv, "ahnm:x:y:s::l::", long_options, &option_index);
        
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
                PC_AI_MODE = 1;
                break;
            
            case 'h':
                help_flag = 1;
                break;
                
            case 's':
                SAVE_DUNGEON = 1;
                size = 1 + strlen(HOME) + strlen("/.rlg327/") + ((optarg && *optarg) ? strlen(optarg) : strlen("dungeon"));
                SAVE_FILE = (char*)calloc(size, sizeof(char));
                sprintf(SAVE_FILE, "%s/.rlg327/", HOME);
                e = mkdir(SAVE_FILE, 0755);
                // clear the error if there is one.
                if(e == -1) {
                    errno = 0;
                } else {
                    logger.i("Creating save directory...");
                }
                strcat(SAVE_FILE, (optarg && *optarg) ? optarg : "dungeon");
                logger.i("Save File Set: %s", SAVE_FILE);
                break;
                
            case 'm':
                num = is_number(optarg);
                if(num <= 1 || num > 50) {
                    logger.w("nummon is out of bounds! Must be in range [1, 50]");
                    fprintf(stderr, "Error: num monsters must be in range [1, 20]");
                    envAPI.exit_gracefully();
                }
                NUM_MONSTERS = num;
                break;
                
            case 'n':
                NCURSES_MODE = 1;
                break;
                
            case 'l':
                LOAD_DUNGEON = 1;
                size = 1 + strlen(HOME) + strlen("/.rlg327/") + ((optarg && *optarg) ? strlen(optarg) : strlen("dungeon"));
                LOAD_FILE = (char*)calloc(size, sizeof(char));
                sprintf(LOAD_FILE, "%s/.rlg327/", HOME);
                e = mkdir(LOAD_FILE, 0755);
                // clear the error if there is one.
                if(e == -1) {
                    errno = 0;
                } else {
                    logger.i("Creating save directory...");
                }
                strcat(LOAD_FILE, (optarg && *optarg) ? optarg : "dungeon");
                logger.i("Load File Set: %s", LOAD_FILE);
                break;
                
            case 'x':
                num = is_number(optarg);
                if(num <= 1 || num > 79) {
                    logger.w("Input X is out of bounds! Must be in range [1, 79]");
                    fprintf(stderr, "Error: starting x coordinate must be in range [1, 79]");
                    envAPI.exit_gracefully();
                }
                X_START = num;
                break;
                
            case 'y':
                num = is_number(optarg);
                if(num <= 1 || num > 20) {
                    logger.w("Input Y is out of bounds! Must be in range [1, 20]");
                    fprintf(stderr, "Error: starting y coordinate must be in range [1, 20]");
                    envAPI.exit_gracefully();
                }
                Y_START = num;
                break;
                
            case '?':
                break;
            
            default:
                envAPI.exit_gracefully();
        }
    }
    
    if(opterr) {
        fprintf(stderr, "Error: invalid flag parsed.\n");
        envAPI.exit_gracefully();
    }
    
    if(help_flag) {
        envAPI.exit_gracefully();
    }
}

static void exit_gracefully() {
    envAPI.cleanup();
    printf("%s", help_text);
    exit(0);
}

static void cleanup() {
    if(SAVE_FILE) {
        free(SAVE_FILE);
    }
    if(LOAD_FILE) {
        free(LOAD_FILE);
    }
    
    characterStoreAPI.teardown();
    eventQueueAPI.teardown();
    dungeonAPI.teardown_dungeon();
    
    if(NCURSES_MODE) {
        endwin();
    }
}

static void move_floors_impl() {
    eventQueueAPI.move_floors();
    dungeonAPI.move_floors();
    characterStoreAPI.move_floors();
    STAIR_FLAG = 0; // reset flag
}

static int is_number(char* str) {
    int val = atoi(str);
    return (val > 0 || (val == 0 && strlen(str) == 1)) ? val : -1;
}

const env_namespace envAPI = {
    parse_args,
    setup_environment,
    exit_gracefully,
    cleanup,
    move_floors_impl
};
    
#ifdef __cplusplus
}
#endif // __cplusplus
