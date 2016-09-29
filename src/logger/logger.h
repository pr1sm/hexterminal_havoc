//
//  logger.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef logger_h
#define logger_h

#include "../env/env.h"

typedef enum logger_mode {
    LOG_T = 1,
    LOG_D = 2,
    LOG_I = 4,
    LOG_W = 8,
    LOG_E = 16,
    LOG_F = 32
} logger_mode;

typedef struct logger_namespace {
    void (*const t)(const char* str, ...);
    void (*const d)(const char* str, ...);
    void (*const i)(const char* str, ...);
    void (*const w)(const char* str, ...);
    void (*const e)(const char* str, ...);
    void (*const f)(const char* str, ...);
    void (*const create)(const char* name);
    void (*const set_modes_enabled)(int modes);
} logger_namespace;
extern logger_namespace const logger;

#endif /* logger_h */
