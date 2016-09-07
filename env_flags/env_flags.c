//
//  env_flags.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/7/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "env_flags.h"
#include "../logger/logger.h"

int DEBUG_MODE = 0;

void setup_environment() {
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
