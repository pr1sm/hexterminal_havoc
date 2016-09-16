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

#include "env.h"
#include "../logger/logger.h"

int DEBUG_MODE = 0;
int LOAD_DUNGEON = 0;
int SAVE_DUNGEON = 0;
char* HOME = "";
char* LOAD_FILE;
char* SAVE_FILE;

static char* help_text = "Usage: hexterm_havoc [options]\n\n"
                         "-l<name>, --load=<name> | Load dungeon with name <name> (in save directory).\n"
                         "-h,       --help        | Print this help message.\n"
                         "-s<name>, --save=<name> | Save the dungeon after loading/generating it with\n"
                         "                        |   name <name> (in save directory).\n";

void env_setup_environment() {
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
    
    if(DEBUG_MODE) {
        logger.i("%%%% RUNNING IN DEBUG MODE %%%%");
    }
    
    logger.i("%%%% ENVIRONMENT SET %%%%");
}

void env_parse_args(int argc, char** argv) {
    if(argc <= 1) return;
    
    int flag;
    int help_flag = 0;
    int e;
    size_t size;
    
    opterr = 0;
    
    while(1) {
        // Setup options
        static struct option long_options[] = {
            {"load", optional_argument, 0, 'l'},
            {"save", optional_argument, 0, 's'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        flag = getopt_long(argc, argv, "hs::l::", long_options, &option_index);
        
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

void env_exit_gracefully() {
    envAPI.cleanup();
    printf("%s", help_text);
    exit(0);
}

void env_cleanup() {
    if(SAVE_FILE) {
        free(SAVE_FILE);
    }
    if(LOAD_FILE) {
        free(LOAD_FILE);
    }
}

const env_namespace envAPI = {
    env_parse_args,
    env_setup_environment,
    env_exit_gracefully,
    env_cleanup
};
