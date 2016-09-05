//
//  logger.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef logger_h
#define logger_h

typedef struct {
    void (*const t)(const char* str, ...);
    void (*const d)(const char* str, ...);
    void (*const i)(const char* str, ...);
    void (*const w)(const char* str, ...);
    void (*const e)(const char* str, ...);
    void (*const f)(const char* str, ...);
    void (*const create)(const char* name);
} logger_namespace;
extern logger_namespace const logger;

#endif /* logger_h */
