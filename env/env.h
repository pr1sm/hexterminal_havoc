//
//  env.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/7/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef env_h
#define env_h

extern int DEBUG_MODE;
extern int LOAD_DUNGEON;
extern int SAVE_DUNGEON;
extern char* HOME;
extern char* LOAD_FILE;
extern char* SAVE_FILE;

typedef struct env_namespace {
    void (*const parse_args)(int argc, char** argv);
    void (*const setup_environment)();
    void (*const exit_gracefully)();
    void (*const cleanup)();
} env_namespace;
extern const env_namespace envAPI;

#endif /* env_h */
