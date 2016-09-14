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

#include "env.h"
#include "../logger/logger.h"

int DEBUG_MODE = 0;
int LOAD_DUNGEON = 0;
int SAVE_DUNGEON = 0;

static char* help_text = "Usage: hexterm_havoc [options]\n\n"
                         "-l,\t--load\tLoad dungeon from the save file.\n"
                         "-h,\t--help\tPrint this help message.\n"
                         "-s,\t--save\tSave the dungeon after loading/generating it.\n";

void env_setup_environment() {
    logger.i("%%%% SETTING ENVIRONMENT %%%%");
    char* env;
    if((env = getenv("ENV"))) {
        if(!strcmp(env, "DEBUG")) {
            DEBUG_MODE = 1;
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
    
    opterr = 0;
    
    while(1) {
        // Setup options
        static struct option long_options[] = {
            {"load", no_argument, 0, 'l'},
            {"save", no_argument, 0, 's'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        flag = getopt_long(argc, argv, "hsl", long_options, &option_index);
        
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
                break;
                
            case 'l':
                LOAD_DUNGEON = 1;
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
    printf("%s", help_text);
    exit(0);
}

const env_namespace envAPI = {
    env_parse_args,
    env_setup_environment,
    env_exit_gracefully
};
